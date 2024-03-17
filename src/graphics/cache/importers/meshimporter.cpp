#include "graphics/cache/importers/meshimporter.hpp"

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
			data->meshes[i].count = scene->mMeshes[i]->mNumFaces * 3;
			data->meshes[i].vertexOffset = data->vertexCount;
			data->meshes[i].indexOffset = data->indexCount;

			data->vertexCount += scene->mMeshes[i]->mNumVertices;
			data->indexCount += data->meshes[i].count;
		}

		for (unsigned int i = 0; i < data->meshes.size(); i++)
		{
			initMesh(data, scene->mMeshes[i]);
		}
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
}