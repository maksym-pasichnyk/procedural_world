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
#include <string>
#include <map>

struct Shader {
	static GLuint find(const std::string& name);

	static std::string source(const std::string& path);
	static GLuint compile(const std::string& source, GLenum type);

	static GLuint create(const std::string &vertex_source, const std::string &fragment_source);
	static GLuint create(const std::string &vertex_source, const std::string &fragment_source, const std::string& geometry_source);

	static GLuint load(const std::string &vertex_file_path, const std::string &fragment_file_path, const std::optional<std::string>& geometry_file_path = std::nullopt);

	static void preload(const std::string& name, const std::string &vertex_file_path, const std::string &fragment_file_path, const std::optional<std::string>& geometry_file_path = std::nullopt);

	static std::map<std::string, GLuint> shaders;
};