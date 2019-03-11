/*
Copyright 2019 Maxim Pasichnyk

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
 */

#include "mesh_builder.h"
#include "mesh.h"

#include <glm/detail/func_geometric.inl>

void MeshBuilder::clear() {
	vertices.clear();
	colors.clear();
	normals.clear();
	indices.clear();
}

Mesh MeshBuilder::build() {
	Mesh mesh;
	mesh.setVertices(vertices);
	mesh.setColors(colors);
	mesh.setNormals(normals);
	mesh.setIndices(indices);
	return mesh;
}

void MeshBuilder::triangle(const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2) {
	auto normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

	auto index = static_cast<uint32_t>(vertices.size());

	vertices.push_back(v0);
	vertices.push_back(v1);
	vertices.push_back(v2);

	colors.push_back(normal);
	colors.push_back(normal);
	colors.push_back(normal);

	normals.push_back(normal);
	normals.push_back(normal);
	normals.push_back(normal);

	indices.push_back(index);
	indices.push_back(index + 1);
	indices.push_back(index + 2);
}

void MeshBuilder::quad(const glm::vec3 &start, const glm::vec3 &width, const glm::vec3 &length) {
	triangle(start, start + width + length, start + width);
	triangle(start, start + length, start + width + length);
}
