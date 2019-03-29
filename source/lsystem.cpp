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

#include "lsystem.h"
#include "mesh_builder.h"

#include <map>
#include <iostream>
#include <stack>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

//todo: generate solid mesh
std::unique_ptr<GameObject> createLObject(const std::string& axiom, const std::map<char, std::string>& rules, size_t iterations, float angle) {
	std::string sentence = axiom;
	while (iterations > 0) {
		iterations--;

		std::string next_sentence{};
		for (auto ch : sentence) {
			auto it = rules.find(ch);
			if (it != rules.end()) {
				next_sentence.append(it->second);
			} else {
				next_sentence.push_back(ch);
			}
		}
		sentence = next_sentence;
	}

	struct state_t {
		glm::vec3 pos;
		glm::vec3 rot;

		glm::vec3 forward() {
			auto matrix = glm::inverse(glm::eulerAngleXYZ(rot.x, rot.y, rot.z));
			return glm::normalize(glm::vec3{matrix[1]});
		}
	};

	std::stack<state_t> stack{};

	state_t state{};
	state.pos = {0, 0, 0};
	state.rot = {0, 0, 0};

	auto push = [&]() -> void {
		stack.push(state);
	};
	auto pop = [&]() -> void {
		state = stack.top();
		stack.pop();
	};

	MeshBuilder builder{};

	for (auto ch : sentence) {
		switch (ch) {
			case 'F':
				builder.indices.push_back(static_cast<uint32_t>(builder.vertices.size()));
				builder.vertices.push_back(state.pos);
				state.pos += state.forward();
				builder.indices.push_back(static_cast<uint32_t>(builder.vertices.size()));
				builder.vertices.push_back(state.pos);
				break;
			case 'f':
				state.pos += state.forward();
				break;
			case '+':
				state.rot.x += angle;
				break;
			case '-':
				state.rot.x -= angle;
				break;
			case '[':
				push();
				break;
			case ']':
				pop();
				break;
			case '&':
				state.rot.y += angle;
				break;
			case '^':
				state.rot.y -= angle;
				break;
			case '<':
				state.rot.z += angle;
				break;
			case '>':
				state.rot.z -= angle;
				break;
			default:
				break;
		}
	}

	auto object = std::make_unique<GameObject>();
	object->mesh = builder.build();
	object->mesh.shader = Shader::find("default");
	object->mesh.mode = GL_LINES;
	return std::move(object);
}