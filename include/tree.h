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

extern std::unique_ptr<GameObject> createTree(const glm::vec3& position);

struct Tree : public GameObject {
private:
	GLuint shader = Shader::find("default");

public:
	GLuint getShader() override {
		return shader;
	}

	void update(double dt) override {
		transform.rotation.y += dt;
	}
};