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

#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <vector>

#include "mesh.h"

struct MeshBuilder {
	std::vector<glm::vec3> vertices{};
	std::vector<glm::vec3> colors{};
	std::vector<glm::vec3> normals{};
	std::vector<uint32_t> indices{};

	void clear();

	Mesh build();

	void triangle(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2);
	void quad(const glm::vec3& start, const glm::vec3& width, const glm::vec3& length);
};