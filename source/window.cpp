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

#include "window.h"
#include <cstdlib>
#include <iostream>
#include <window.h>

Window::Window(int width, int height, const char *title) : h_width{width * 0.5}, h_height{height * 0.5} {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	window = glfwCreateWindow(width, height, title, nullptr, nullptr);

	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);

	glfwGetFramebufferSize(window, &width, &height);

	glfwSetWindowFocusCallback(window, Window::focusChange);

	glfwSetKeyCallback(window, Window::processInput);

	glewInit();

	glViewport(0, 0, width, height);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	std::atexit(glfwTerminate);
}

void Window::clear(float r, float g, float b, float a) {
	glClearColor(r, g, b, a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::processInput(GLFWwindow* window, int key, int code, int action, int mods) {

}

void Window::focusChange(GLFWwindow* window, int focus) {
	auto self = Module::getInstance<Window>(Module::WINDOW);

	if (focus) {
		self->init_mouse = true;
		self->dx = 0;
		self->dy = 0;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	} else {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
	self->focus = static_cast<bool>(focus);
}