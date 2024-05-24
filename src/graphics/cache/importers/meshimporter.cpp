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
		const aiScene* scene = m_importer.ReadFile(filePath.string(), /*aiProcess_GenSmoothNormals |*/ aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType | aiProcess_FlipWindingOrder | aiProcess_PreTransformVertices);
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
			auto matName = i == 0 ? std::format("{}-material", data->name) : std::format("{}{}-material", data->name, i);
			auto filePath = "resources/shaders/lit.shader";
			auto shaderName = "lit";
			auto mat_source = material_source{ .name = matName, .shaderPath = filePath, .shaderName = shaderName };
			auto albedoTextures = initMaterial(scene, material, aiTextureType_DIFFUSE, "default-albedo");
			for (auto texture : albedoTextures)
			{
				texture->bufferRegister = 0;
				texture->type = ParamType::Texture;
				mat_source.parameters.emplace("Albedo", texture);
			}
			auto roughnessTextures = initMaterial(scene, material, aiTextureType_SPECULAR, "default-roughness");
			for (auto texture : roughnessTextures)
			{
				texture->bufferRegister = 1;
				texture->type = ParamType::Texture;
				mat_source.parameters.emplace("Roughness", texture);
			}
			auto normalTextures = initMaterial(scene, material, aiTextureType_NORMALS, "default-normal");
			for (auto texture : normalTextures)
			{
				texture->bufferRegister = 2;
				texture->type = ParamType::Texture;
				mat_source.parameters.emplace("Normals", texture);
			}
			auto heightTextures = initMaterial(scene, material, aiTextureType_DISPLACEMENT, "default-height");
			for (auto texture : heightTextures)
			{
				texture->bufferRegister = 3;
				texture->type = ParamType::Texture;
				mat_source.parameters.emplace("Height", texture);
			}
			auto metalTextures = initMaterial(scene, material, aiTextureType_METALNESS, "default-metallic");
			for (auto texture : metalTextures)
			{
				texture->bufferRegister = 4;
				texture->type = ParamType::Texture;
				mat_source.parameters.emplace("Metallic", texture);
			}
			auto ambientTextures = initMaterial(scene, material, aiTextureType_AMBIENT_OCCLUSION, "default-ao");
			for (auto texture : ambientTextures)
			{
				texture->bufferRegister = 5;
				texture->type = ParamType::Texture;
				mat_source.parameters.emplace("AmbientOcclusion", texture);
			}
			auto emissiveTextures = initMaterial(scene, material, aiTextureType_EMISSIVE, "default-emissive");
			for (auto texture : emissiveTextures)
			{
				texture->bufferRegister = 6;
				texture->type = ParamType::Texture;
				mat_source.parameters.emplace("Emissive", texture);
			}

			auto matAsset = ast::AssetCache<material_source>::createAssetFromMemory(matName, mat_source, ast::import_settings<material_source>{});
			auto matHandle = MaterialCache::loadMaterial(matName, matAsset);
			matHandle->data = material_data
			{
				.diffuseColor = math::vec4(1.0,1.0,1.0,1.0),
				.hasAlbedo = (unsigned int)(albedoTextures.size() > 0),
				.hasRoughness = (unsigned int)(roughnessTextures.size() > 0),
				.hasNormal = (unsigned int)(normalTextures.size() > 0),
				.hasHeight = (unsigned int)(heightTextures.size() > 0),
				.hasMetallic = (unsigned int)(metalTextures.size() > 0),
				.hasAmbientOcclusion = (unsigned int)(ambientTextures.size() > 0),
				.hasEmissive = (unsigned int)(emissiveTextures.size() > 0),

			};
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
	std::vector<material_parameter<std::string>*> MeshImporter::initMaterial(const aiScene* scene, aiMaterial* mat, aiTextureType type, const std::string& defaultTexName )
	{
		std::vector<material_parameter<std::string>*> textures;
		auto texCount = mat->GetTextureCount(type);
		for (unsigned int j = 0; j < texCount; j++)
		{
			aiString strPath;
			mat->GetTexture(type, j, &strPath);
			if ('*' == strPath.data[0])//embedded texture
			{
				auto idx = std::stoi(std::format("{}{}", strPath.data[1], strPath.data[2]));
				auto texture = scene->mTextures[idx];
				auto textureName = std::format("{}-{}", mat->GetName().C_Str(), texture->mFilename.C_Str());
				math::ivec2 resolution = math::ivec2(0, 0);
				int channels = 0;
				unsigned char* textureData = nullptr;
				if (texture->mHeight == 0)
					textureData = stbi_load_from_memory(reinterpret_cast<unsigned char*>(texture->pcData), texture->mWidth, &resolution.x, &resolution.y, &channels, 0);
				else
					textureData = stbi_load_from_memory(reinterpret_cast<unsigned char*>(texture->pcData), texture->mWidth * texture->mHeight, &resolution.x, &resolution.y, &channels, 0);
				auto tex_source = texture_source{ .name = textureName, .resolution = resolution,.channels = channels, .data = textureData };
				auto textureAsset = ast::AssetCache<texture_source>::createAssetFromMemory(textureName, tex_source, ast::import_settings<texture_source>{});
				auto texHandle = TextureCache::createTexture2D(textureName, textureAsset);
				textures.emplace_back(new material_parameter<std::string>{ .value = texHandle->getName() });
				stbi_image_free(textureData);
			}
		}

		if (textures.size() < 1)
		{
			auto textureAsset = ast::AssetCache<texture_source>::createAsset(defaultTexName, fs::path(std::format("resources/textures/default/{}.png", defaultTexName)), ast::import_settings<texture_source>{});
			auto texHandle = TextureCache::createTexture2D(defaultTexName, textureAsset);
			textures.emplace_back(new material_parameter<std::string>{ .value = texHandle->getName() });
		}
		return textures;
	}
}