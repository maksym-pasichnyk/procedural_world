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

#include "camera.h"
#include "window.h"

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

glm::mat4x4 Camera::matrix() {
	return projection() * transform.matrix();
}

glm::mat4x4 Camera::projection() {
	return glm::perspective(glm::radians(fov), aspect, near, far);
}

void Camera::update(double dt) {
	auto window = Module::getInstance<Window>(Module::WINDOW);

	auto [dx, dy] = window->getMouseDelta();

	transform.rotation.x += dy * dt * 0.5f;
	transform.rotation.y += dx * dt * 0.5f;

	auto matrix = glm::inverse(transform.matrix());
	auto right = glm::normalize(glm::vec3{matrix[0]});
	auto forward = glm::normalize(glm::vec3{matrix[2]});

	float speed = 50.0f;
	if (window->getKeyState(GLFW_KEY_W) == GLFW_PRESS) {
		transform.position -= forward * (float)(dt * speed);
	}
	if (window->getKeyState(GLFW_KEY_S) == GLFW_PRESS) {
		transform.position += forward * (float)(dt * speed);
	}
	if (window->getKeyState(GLFW_KEY_D) == GLFW_PRESS) {
		transform.position += right * (float)(dt * speed);
	}
	if (window->getKeyState(GLFW_KEY_A ) == GLFW_PRESS) {
		transform.position -= right * (float)(dt * speed);
	}
}
