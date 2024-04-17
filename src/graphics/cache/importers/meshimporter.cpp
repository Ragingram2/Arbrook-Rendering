#include "graphics/cache/importers/meshimporter.hpp"
#include "graphics/cache/materialcache.hpp"
#include "graphics/data/materialsource.hpp"
#include "graphics/cache/texturecache.hpp"
#include <stb/stb_image.h>

namespace fs = std::filesystem;
namespace ast = rythe::core::assets;

template<>
std::unordered_map<rsl::id_type, std::unique_ptr<ast::AssetImporter<rythe::rendering::mesh>>> ast::AssetCache<rythe::rendering::mesh>::m_importers;
template<>
std::unordered_map<rsl::id_type, std::unique_ptr<rythe::rendering::mesh>> ast::AssetCache<rythe::rendering::mesh>::m_assets;
template<>
std::unordered_map<rsl::id_type, std::string> ast::AssetCache<rythe::rendering::mesh>::m_names;

namespace rythe::rendering
{
	bool MeshImporter::canLoad(fs::path filePath)
	{
		return m_importer.IsExtensionSupported(filePath.extension().string().c_str());
	}

	ast::asset_handle<mesh> MeshImporter::load(rsl::id_type id, fs::path filePath, mesh* data, const ast::import_settings<mesh>& settings)
	{
		const aiScene* scene = m_importer.ReadFile(filePath.string(), aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType | aiProcess_FlipWindingOrder);
		if (!scene)
		{
			log::error("Problem in loading mesh file");
			return { 0, nullptr };
		}
		data->name = filePath.stem().string();

		if (!scene->HasMeshes())
		{
			log::error("The scene we tried to load did not contain any meshes, curious");
			return{ 0, nullptr };
		}

		data->meshes.reserve(scene->mNumMeshes);
		data->vertexCount = 0;
		data->indexCount = 0;

		for (unsigned int i = 0; i < scene->mNumMeshes; i++)
		{
			data->meshes.emplace_back();
			data->meshes[i].materialIdx = scene->mMeshes[i]->mMaterialIndex;
			data->meshes[i].name = scene->mMeshes[i]->mName.C_Str();
			data->meshes[i].count = scene->mMeshes[i]->mNumFaces * 3;
			data->meshes[i].vertexOffset = data->vertexCount;
			data->meshes[i].indexOffset = data->indexCount;

			data->vertexCount += scene->mMeshes[i]->mNumVertices;
			data->indexCount += data->meshes[i].count;

			if (scene->mNumMaterials < 1) continue;

			aiMaterial* material = scene->mMaterials[scene->mMeshes[i]->mMaterialIndex];
			auto matAssets = ast::AssetCache<material_source>::getAssets();
			material_source mat_source;
			mat_source.name = std::format("{}-{}-material", data->name, data->meshes[i].name);
			mat_source.filePath = "resources/shaders/lit.shader";
			mat_source.shaderName = "lit";
			initMaterial(scene, material, aiTextureType_DIFFUSE, mat_source);
			auto matAsset = ast::AssetCache<material_source>::createAssetFromMemory(mat_source.name, &mat_source, ast::import_settings<material_source>{});
			auto matHandle = MaterialCache::loadMaterial(mat_source.name, matAsset);
			matAssets = ast::AssetCache<material_source>::getAssets();
			data->materialIds[data->meshes[i].materialIdx] = matHandle.m_id;
		}

		for (unsigned int i = 0; i < data->meshes.size(); i++)
		{
			initMesh(data, scene->mMeshes[i]);
		}

		return { id, data };
	}

	ast::asset_handle<mesh> MeshImporter::loadFromMemory(rsl::id_type id, mesh* data, const ast::import_settings<mesh>& settings)
	{
		return { id, data };
	}

	void MeshImporter::write(fs::path filePath, mesh* data)
	{
		//std::ofstream o(filePath);
		//nlohmann::json j;
		//ns::to_json(j, *data);
		//o << std::setw(4) << j << std::endl;
	}

	void MeshImporter::free(mesh&) {}

	void MeshImporter::initMesh(mesh* data, const aiMesh* _mesh)
	{
		const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
		auto count = _mesh->mNumVertices;

		for (int i = 0; i < count; i++)
		{
			const aiVector3D& vertex = _mesh->mVertices[i];
			const aiVector3D& normal = _mesh->mNormals[i];
			const aiVector3D& tangent = _mesh->HasTangentsAndBitangents() ? _mesh->mTangents[i] : Zero3D;
			const aiVector3D texcoord = _mesh->HasTextureCoords(0) ? _mesh->mTextureCoords[0][i] : Zero3D;

			data->vertices.push_back(math::vec4(vertex.x, vertex.y, vertex.z, 1.0f));
			data->normals.push_back(math::vec3(normal.x, normal.y, normal.z));
			data->texCoords.push_back(math::vec2(texcoord.x, texcoord.y));
			data->tangents.push_back(math::vec3(tangent.x, tangent.y, tangent.z));
		}

		if (_mesh->HasVertexColors(0))
		{
			auto s_colors = _mesh->mColors[0];

			for (int i = 0; i < count; i++)
			{
				data->colors.push_back(math::vec4(s_colors[i].r, s_colors[i].g, s_colors[i].b, s_colors[i].a));
			}
		}

		if (_mesh->HasFaces())
			for (int i = 0; i < _mesh->mNumFaces; i++)
			{
				auto indexCount = _mesh->mFaces[i].mNumIndices;
				auto s_indices = _mesh->mFaces[i].mIndices;
				for (int j = 0; j < indexCount; j++)
				{
					data->indices.push_back(s_indices[j]);
				}
			}
	}
	void MeshImporter::initMaterial(const aiScene* scene, aiMaterial* mat, aiTextureType type, material_source& mat_source)
	{
		auto texCount = mat->GetTextureCount(type);
		for (unsigned int j = 0; j < texCount; j++)
		{
			aiString strPath;
			mat->GetTexture(type, j, &strPath);
			if ('*' == strPath.data[0])//embedded texture
			{
				auto idx = std::stoi(std::format("{}{}", strPath.data[1], strPath.data[2]));
				auto texture = scene->mTextures[idx];
				texture_source tex_source;
				tex_source.name = texture->mFilename.C_Str();
				if (texture->mHeight == 0)
					tex_source.data = stbi_load_from_memory(reinterpret_cast<unsigned char*>(texture->pcData), texture->mWidth, &tex_source.resolution.x, &tex_source.resolution.y, &tex_source.channels, 0);
				else
					tex_source.data = stbi_load_from_memory(reinterpret_cast<unsigned char*>(texture->pcData), texture->mWidth * texture->mHeight, &tex_source.resolution.x, &tex_source.resolution.y, &tex_source.channels, 0);
				auto textureAsset = ast::AssetCache<texture_source>::createAssetFromMemory(tex_source.name, &tex_source, ast::import_settings<texture_source>{});
				auto texHandle = TextureCache::createTexture2D(tex_source.name, textureAsset);
				auto textureAssets = ast::AssetCache<texture_source>::getAssets();
				mat_source.textures.push_back(texHandle->getName());
				stbi_image_free(tex_source.data);
			}
		}
	}
}