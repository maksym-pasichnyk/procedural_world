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


#include "tree.h"

#include "mesh_builder.h"

#include <random>
#include <chrono>

#include <iostream>

#include <functional>

//#include <glm/detail/func_trigonometric.inl>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

struct leaf_t {
	glm::vec3 pos;
	bool reached = false;
	leaf_t(const glm::vec3& pos) : pos{pos} {}
};

struct branch_t {
	std::optional<glm::vec3> start;
	glm::vec3 pos;
	glm::vec3 dir = {0, 0, 0};
	size_t count = 0;

	explicit branch_t(const glm::vec3& pos, const std::optional<glm::vec3>& start = std::nullopt) : pos{pos}, start{start} {}

	void reset() {
		dir = {0, 0, 0};
		count = 0;
	}
};

branch_t grow_branch(const branch_t &branch, const glm::vec3 &dir) {
	return branch_t{branch.pos + dir, branch.pos};
}

//todo: generate solid mesh
std::unique_ptr<GameObject> createTree(const glm::vec3& position) {
	std::default_random_engine random_engine{static_cast<unsigned long>(std::chrono::high_resolution_clock::now().time_since_epoch().count())};

	auto random_value = [&](double max = 1.0) -> double {
		return (static_cast<double>(random_engine()) / static_cast<double>(std::default_random_engine::max())) * max;
	};

	MeshBuilder builder{};

	auto create_branch_mesh = [&](const glm::vec3& start, const glm::vec3& end, float radius, float segment_height, int segments) {
		auto index = builder.vertices.size();
//
//		builder.vertices.push_back(start);
//		builder.vertices.push_back(end);
//
//		builder.indices.push_back(index);
//		builder.indices.push_back(index + 1);

		float distance = glm::distance(end, start);
		segment_height = distance / 10.0f;
//
		int segments_per_height = static_cast<int>(distance / segment_height);

		auto dir = end - start;
		auto part = dir / static_cast<float>(segments_per_height);

		auto n = glm::normalize(dir);


		float segment = 360.0f / segments;
		auto s = start;
		for (int j = 0; j < segments_per_height; j++) {
			for (int i = 0; i < segments; i++) {
				float angle = glm::radians(segment * i);

				glm::vec3 point{radius * glm::cos(angle), static_cast<float>(j) * segment_height, radius * glm::sin(angle)};

//				point = glm::cross(n, point);



//
//				point = rotate * glm::vec4(point, 1.0f);




				glm::mat4x4 translate = glm::translate(glm::mat4x4(1.0f), point);
				glm::mat4x4 rotate = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), n, glm::vec3(0.0f, 1.0f, 0.0f));
				point = translate * rotate * glm::vec4(point, 1.0f);

				float x = s.x + point.x;
				float y = s.y + point.y;
				float z = s.z + point.z;

//				float y = y0 * cos_alpha + z0 * sin_alpha + start.y;
//				float z = z0 * cos_alpha - y0 * sin_alpha + start.z;

				builder.indices.push_back(builder.vertices.size());
				builder.vertices.emplace_back(x, y, z);
			}
			s += part;
		}

//		for (int j = 0; j < segments_per_height; j++) {
//			for (int i = 0; i < segments; i++) {
//				auto corner0 = index + i + j * segments;
//				auto corner1 = corner0 + segments;
//				auto corner2 = index + (1 + i) % segments + j * segments;
//				auto corner3 = corner2 + segments;
//
//				builder.indices.push_back(corner0);
//				builder.indices.push_back(corner1);
//				builder.indices.push_back(corner2);
//
//				builder.indices.push_back(corner2);
//				builder.indices.push_back(corner1);
//				builder.indices.push_back(corner3);
//			}

//				builder.indices.push_back(index + j);
//		}
	};

	auto createBranch = [&](const branch_t& branch) -> void {
		if (branch.start) {
			create_branch_mesh(branch.start.value(), branch.pos, 0.5f, 0.1f, 10);
		}
	};

	std::vector<leaf_t> leaves{};
	std::vector<branch_t> branches{};

	for (int i = 0; i < 500; i++) {
		double x = random_value(50) - 25;
		double y = random_value(50) - 25;
		double z = random_value(50) - 25;

		leaves.emplace_back(glm::vec3{x, y, z});
	}

	branch_t root {glm::vec3{0, -50, 0}};

	branches.push_back(root);

	constexpr double min_dist = 10.0;
	constexpr double max_dist = 25.0;

	auto current = &root;

	bool found = false;
	while (!found) {
		for (auto& leaf : leaves) {
			auto dist = glm::distance(leaf.pos, current->pos);

			if (dist < max_dist) {
				found = true;
				break;
			}
		}

		if (!found) {
			branches.push_back(grow_branch(*current, glm::vec3{0.0f, 5.0f, 0.0f}));
			current = &branches.back();
		}
	}

	for (int i = 0; i < 1000; i++) {
		if (leaves.empty()) break;

		for (auto &leaf : leaves) {
			branch_t* closest = nullptr;
			double m_dist = 0;

			for (auto& branch : branches) {
				auto dist = glm::distance(leaf.pos, branch.pos);

				if (dist > max_dist) {
					continue;
				}

				if (dist < min_dist) {
					leaf.reached = true;
					break;
				}

				if (!closest || (m_dist > dist)) {
					closest = &branch;
					m_dist = dist;
				}
			}

			if (!leaf.reached && closest) {
				auto dir = glm::normalize(leaf.pos - closest->pos);
				closest->dir += dir;
				closest->count++;
			}
		}

		leaves.erase(std::remove_if(leaves.begin(), leaves.end(), [](const leaf_t &leaf) { return leaf.reached; }), leaves.end());

		for (auto& branch : branches) {
			if (branch.count > 0) {
				auto dir = glm::normalize(branch.dir / static_cast<float>(branch.count));

				branches.push_back(grow_branch(branch, dir));
			}

			branch.reset();
		}
	}

	std::cout << branches.size() << std::endl;

	for (auto const& branch : branches) {
		createBranch(branch);
	}

	auto object = std::make_unique<GameObject>();
	object->mesh = builder.build();
	object->mesh.shader = Shader::find("default");
	object->mesh.mode = GL_LINES;
	object->transform.position = position;
	return std::move(object);
}