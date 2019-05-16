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
#include <thread>

#include <vector>
#include <map>

#include "window.h"
#include "timer.h"
#include "mesh.h"
#include "shader.h"
#include "camera.h"
#include "planet.h"
#include "input.h"
#include "tree.h"
#include "lsystem.h"
#include "proctree.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <mesh_builder.h>

std::vector<std::unique_ptr<GameObject>> objects{};

//std::unique_ptr<GameObject> createTube(const glm::vec3& start, const glm::vec3& end) {
//
//}

void orthoNormalize(glm::vec3& normal, glm::vec3& tangent)  {
	normal = glm::normalize(normal);
	tangent = glm::normalize(tangent - normal * glm::dot(tangent, normal));
}

glm::quat lookRotation(glm::vec3 lookAt, glm::vec3 up) {
	auto forward = glm::normalize(lookAt);

	orthoNormalize(up, forward);

	auto right = glm::cross(up, forward);

	float w = sqrtf(1.0f + right.x + up.y + forward.z) * 0.5f;

	glm::quat ret{};
	float w4_recip = 1.0f / (4.0f * w);
	ret.x = (forward.y - up.z) * w4_recip;
	ret.y = (right.z - forward.x) * w4_recip;
	ret.z = (up.x - right.y) * w4_recip;
	ret.w = w;
	return ret;
}

int main() {
	Window window{640, 480, "Procedural world"};
	Input input{};
	Timer timer{};

	Module::registerInstance(&window);
	Module::registerInstance(&input);

	Shader::preload("default", "assets/shaders/default/vertex.glsl", "assets/shaders/default/fragment.glsl");
	Shader::preload("default_wood", "assets/shaders/default/vertex.glsl", "assets/shaders/default/fragment.glsl");

	objects.push_back(createPlanet({0, 0, 0}, 5, 30, 5));
	auto planet = static_cast<Planet*>(objects.back().get());

	std::vector<GameObject*> trees{};


//	auto lookRotation = [up = glm::vec3{0, 1, 0}](const glm::vec3& point, const glm::vec3& direction) -> glm::vec3 {
//		return glm::eulerAngles(glm::quatLookAt(glm::normalize(point), glm::normalize(glm::cross(up, glm::normalize(direction)))));
//	};


	for (int phi = 0; phi < 360; phi += 30) {
		for (int theta = 0; theta < 360; theta += 30) {
			float r_theta = glm::radians((float)theta);
			float r_phi = glm::radians((float)phi);

			float x = planet->radius * std::sin(r_theta) * std::cos(r_phi);
			float y = planet->radius * std::sin(r_theta) * std::sin(r_phi);
			float z = planet->radius * std::cos(r_theta);

			auto point = planet->getPoint({x, y, z});
//			auto tree = createProcTree(glm::normalize(point) * 100.0f);
//
//			tree->transform.rotation = glm::vec3{0, 0, 0};
//			trees.push_back(tree.get());
//					lookRotation(planet->transform.position - point, point);
//			objects.push_back(std::move(tree));
		}
	}

	Camera camera{};
	camera.far = 100000.0f;
	camera.transform.position = {100, 30, 0};
	camera.transform.rotation = {0, -1.5577, 0};

	timer.reset();
	while (!window.shouldClose()) {
		auto dt = timer.elapsed();

		window.clear(0.4, 0.6, 0.8, 1);
		camera.update(dt);

		auto world_matrix = camera.matrix();

		for (auto const& obj : objects) {
			auto model_matrix = obj->transform.matrix();

			glUseProgram(obj->mesh.shader);
			glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(world_matrix));
			glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr(model_matrix));
			obj->mesh.draw();
		}

		window.swap();
		window.pollEvents();

		std::this_thread::sleep_for(std::chrono::milliseconds(2));
	}

	return 0;
}