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

#include <memory>
#include "gameobject.h"

struct Planet : public GameObject {
	std::vector<glm::vec3> vertices;
	std::vector<uint32_t> indices;

	int level_of_detail;
	float radius;
	float height_variation;


	Planet(const std::vector<glm::vec3>& vertices, const std::vector<uint32_t>& indices, int level_of_detail, float radius, float height_variation) : vertices(vertices), indices(indices), level_of_detail(level_of_detail), radius(radius), height_variation(height_variation) {}

	glm::vec3 getPoint(const glm::vec3& point);
};

extern std::unique_ptr<GameObject> createPlanet(const glm::vec3& position, int level_of_detail, float radius, float height_variation);