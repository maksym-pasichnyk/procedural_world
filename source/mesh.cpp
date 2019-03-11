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

#include <iostream>
#include "mesh.h"

Mesh::Mesh() {
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glGenBuffers(1, &CBO);
	glGenBuffers(1, &NBO);
	glGenBuffers(1, &IBO);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	glBindBuffer(GL_ARRAY_BUFFER, CBO);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	glBindBuffer(GL_ARRAY_BUFFER, NBO);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, nullptr);


//	glDisableVertexAttribArray(0);
//	glDisableVertexAttribArray(1);
//	glDisableVertexAttribArray(2);

	glBindVertexArray(0);
}

void Mesh::setVertices(const std::vector<glm::vec3> &vertices) {
	this->vertices = vertices;

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
	glBindVertexArray(0);
}

void Mesh::setColors(const std::vector<glm::vec3> &colors) {
	this->colors = colors;

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, CBO);
	glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), colors.data(), GL_STATIC_DRAW);
	glBindVertexArray(0);
}

void Mesh::setNormals(const std::vector<glm::vec3> &normals) {
	this->normals = normals;

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, NBO);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);
	glBindVertexArray(0);
}

void Mesh::setIndices(const std::vector<uint32_t> &indices) {
	this->indices = indices;

	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);
	glBindVertexArray(0);
}

void Mesh::draw() {
	glBindVertexArray(VAO);
	glDrawElements(mode, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, nullptr);
}
