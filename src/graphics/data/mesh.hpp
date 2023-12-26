#pragma once
#include <vector>

#include <assimp/scene.h>

#include "core/math/math.hpp"
#include "core/logging/logging.hpp"


namespace rythe::rendering
{
	struct sub_mesh
	{
		std::string name;
		rsl::size_type count;
		rsl::size_type vertexOffset;
		rsl::size_type indexOffset;
		rsl::int32 materialIdx;
	};

	struct mesh
	{
		rsl::size_type vertexCount;
		rsl::size_type indexCount;

		std::string name;
		std::vector<math::vec4> vertices;
		std::vector<math::vec4> colors;
		std::vector<math::vec3> normals;
		std::vector<math::vec2> texCoords;
		std::vector<unsigned int> indices;
		//std::vecotr<material_data> materials;
		std::vector<sub_mesh> meshes;

		mesh() = default;
		mesh(const mesh& msh) : name(msh.name), vertices(msh.vertices), colors(msh.colors), normals(msh.normals), texCoords(msh.texCoords), indices(msh.indices), meshes(msh.meshes) {}
		mesh(const aiScene* scene)
		{
			load(scene);
		}

		void load(const aiScene* scene)
		{
			//name = std::string(scene->mName.C_Str());
			if (!scene->HasMeshes())
			{
				log::error("The scene we tried to load did not contain any meshes, curious");
				return;
			}

			meshes.reserve(scene->mNumMeshes);
			vertexCount = 0;
			indexCount = 0;
			log::debug("Loading {} Meshe(s)", scene->mNumMeshes);
			for (unsigned int i = 0; i < scene->mNumMeshes; i++)
			{
				meshes.emplace_back();
				meshes[i].materialIdx = scene->mMeshes[i]->mMaterialIndex;
				meshes[i].count = scene->mMeshes[i]->mNumFaces * 3;
				meshes[i].vertexOffset = vertexCount;
				meshes[i].indexOffset = indexCount;

				vertexCount += scene->mMeshes[i]->mNumVertices;
				indexCount += meshes[i].count;
			}

			for (unsigned int i = 0; i < meshes.size(); i++)
			{
				initMesh(scene->mMeshes[i]);
			}

		}

		void initMesh(const aiMesh* _mesh)
		{
			const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
			auto count = _mesh->mNumVertices;

			for (int i = 0; i < count; i++)
			{
				const aiVector3D& vertex = _mesh->mVertices[i];
				const aiVector3D& normal = _mesh->mNormals[i];
				const aiVector3D texcoord = _mesh->HasTextureCoords(0) ? _mesh->mTextureCoords[0][i] : Zero3D;

				vertices.push_back(math::vec4(vertex.x, vertex.y, vertex.z, 1.0f));
				normals.push_back(math::vec3(normal.x, normal.y, normal.z));
				texCoords.push_back(math::vec2(texcoord.x, texcoord.y));
			}

			if (_mesh->HasVertexColors(0))
			{
				auto s_colors = _mesh->mColors[0];

				for (int i = 0; i < count; i++)
				{
					colors.push_back(math::vec4(s_colors[i].r, s_colors[i].g, s_colors[i].b, s_colors[i].a));
				}
			}

			if (_mesh->HasFaces())
				for (int i = 0; i < _mesh->mNumFaces; i++)
				{
					auto indexCount = _mesh->mFaces[i].mNumIndices;
					auto s_indices = _mesh->mFaces[i].mIndices;
					for (int j = 0; j < indexCount; j++)
					{
						indices.push_back(s_indices[j]);
					}
				}
		}
	};
}