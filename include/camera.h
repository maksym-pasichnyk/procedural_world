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
#include "transform.h"

struct Camera {
	Transform transform;

	float near = 0.1f;
	float far = 2000.0f;
	float aspect = 4.0f/3.0f;
	float fov = 45.0f;

	glm::mat4x4 matrix();
	glm::mat4x4 projection();

	void update(double dt);
};