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

#include <glm/gtc/type_ptr.hpp>

std::vector<std::unique_ptr<GameObject>> objects{};

int main() {
	Window window{640, 480, "Procedural world"};
	Input input{};
	Timer timer{};

	Module::registerInstance(&window);
	Module::registerInstance(&input);

	Shader::shaders["default"] = Shader::load("assets/shaders/default/vertex.glsl", "assets/shaders/default/fragment.glsl");

//	objects.push_back(createPlanet({0, 0, 0}, 5, 3, 1));
//	objects.push_back(createTree({0, 0, 0}));
	objects.push_back(createLObject("AB", {
			{'A', "[F[+FCA][-FCA]]"},
			{'B', "[F[+FCB][-FCB]]"},
	}, 4, glm::radians(20.0f)));

	Camera camera{};
	camera.transform.position = {200, 30, 0};
	camera.transform.rotation = {0, - M_PI / 2.0f, 0};

	timer.reset();
	while (!window.shouldClose()) {
		auto dt = timer.elapsed();

		window.clear(0.4, 0.6, 0.8, 1);
		camera.update(dt);

		for (auto const& obj : objects) {
			obj->update(dt);
		}

		auto world_matrix = camera.matrix();

		for (auto const& obj : objects) {
			auto model_matrix = obj->transform.matrix();

			glUseProgram(obj->getShader());
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