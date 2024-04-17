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
		rsl::id_type materialIdx;
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
		std::vector<math::vec3> tangents;
		std::vector<unsigned int> indices;
		std::unordered_map<rsl::id_type,rsl::id_type> materialIds;
		std::vector<sub_mesh> meshes;

		mesh() = default;
		mesh(const mesh& msh) : name(msh.name), vertices(msh.vertices), colors(msh.colors), normals(msh.normals), texCoords(msh.texCoords), indices(msh.indices), meshes(msh.meshes) {}
	};
}