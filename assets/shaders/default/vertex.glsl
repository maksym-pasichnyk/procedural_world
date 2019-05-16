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

#version 330 core

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_color;
layout(location = 2) in vec3 vertex_normal;

out vec3 frag_position;
out vec3 vert_position;
out vec3 frag_normal;
out vec3 frag_color;

uniform mat4 world_transform;
uniform mat4 model_transform;

void main() {
	vec4 position = model_transform * vec4(vertex_position, 1.0);

    gl_Position = world_transform * position;

    vert_position = vertex_position;
    frag_position = position.xyz;
    frag_normal = normalize(frag_position);
    frag_color = vertex_color;
}