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

out vec3 color;

in vec3 frag_position;
in vec3 frag_normal;
in vec3 frag_color;

vec3 lightPos = vec3(2000, 0, 0);
vec3 lightColor = vec3(1, 1, 1);

void main() {
    float ambientStrength = 0.5f;
    vec3 ambient = ambientStrength * lightColor;

    vec3 lightDir = normalize(lightPos - frag_position);
    float diff = max(dot(frag_normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

	color = (ambient + diffuse) * frag_color;
}