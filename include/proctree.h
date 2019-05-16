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
#include <optional>
#include "gameobject.h"

struct TreeProperties {
	float clump_max = 0.454f;
	float clump_min = 0.404f;
	float length_falloff_factor = 0.85f;
	float length_falloff_power = 0.99f;
	float branch_factor = 2.45f;
	float radius_falloff_rate = 0.73f;
	float climb_rate = 0.371f;
	float trunk_kink = 0.093f;
	float max_radius = 0.139f * 5;
	int tree_steps = 5;
	float taper_rate = 0.947f;
	float twist_rate = 3.02f;
	int segments = 6;
	int levels = 5;
	float sweep_amount = 0.01f * 5;
	float initial_branch_length = 0.49f * 10;
	float trunk_length = 2.4f * 5;
	float drop_amount = -0.1f;
	float grow_amount = 0.235f;
	float twig_scale = 0.39f;
	int seed = 262;
	int rseed = 0;

	float next(float fixed) {
		if (fixed == 0) {
			fixed = (float) rseed++;
		}
		return std::abs(std::cos(fixed + fixed * fixed));
	}
};

std::unique_ptr<GameObject> createProcTree(const glm::vec3& position, const std::optional<TreeProperties>& properties = std::nullopt);