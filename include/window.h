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

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <utility>
#include "module.h"

struct Window : Module {
public:
	Window(int width, int height, const char* title);

	bool shouldClose() const {
		return static_cast<bool>(glfwWindowShouldClose(window));
	}

	void clear(float r, float g, float b, float a = 1.0f);

	void pollEvents() {
		glfwPollEvents();

		if (focus) {
			if (!init_mouse) {
				double mouse_x, mouse_y;
				glfwGetCursorPos(window, &mouse_x, &mouse_y);
				dx = mouse_x - h_width;
				dy = mouse_y - h_height;
			}
			init_mouse = false;
			glfwSetCursorPos(window, h_width, h_height);
		}
	}

	void swap() {
		glfwSwapBuffers(window);
	}

	void close() {
		glfwSetWindowShouldClose(window, 1);
	}

	std::pair<double, double> getMouseDelta() {
		return std::make_pair(dx, dy);
	}

	int getKeyState(int key) {
		return glfwGetKey(window, key);
	}

	void* getNativePointer() {
		return window;
	}

	ModuleType getModuleType() override {
		return Module::WINDOW;
	}
private:
	GLFWwindow* window = nullptr;
	bool focus = false;
	bool init_mouse = false;
	double dx = 0;
	double dy = 0;
	double h_width = 0;
	double h_height = 0;

	static void processInput(GLFWwindow* window, int key, int code, int action, int mods);
	static void focusChange(GLFWwindow* window, int focus);
};