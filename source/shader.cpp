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

#include "shader.h"

#include <fstream>
#include <sstream>
#include <GL/glew.h>

std::map<std::string, GLuint> Shader::shaders{};

GLuint Shader::find(const std::string &name) {
	return shaders[name];
}

std::string Shader::source(const std::string &path) {
	std::ifstream file(path, std::ios::in);
	std::stringstream stream{};
	stream << file.rdbuf();
	file.close();
	return stream.str();
}

GLuint Shader::compile(const std::string &source, GLenum type) {
	auto data = source.data();
	auto size = GLint(source.size());

	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &data, &size);
	glCompileShader(shader);
//	GLint status = GL_FALSE;
	GLint length = 0;

//	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
	if (length > 0) {
		char message[length];
		glGetShaderInfoLog(shader, length, nullptr, message);
		fprintf(stdout, "%s\n", message);
	}

	return shader;
}

GLuint Shader::create(const std::string &vertex_source, const std::string &fragment_source) {
	auto vertex = compile(vertex_source, GL_VERTEX_SHADER);
	auto fragment = compile(fragment_source, GL_FRAGMENT_SHADER);

	GLuint program = glCreateProgram();
	glAttachShader(program, vertex);
	glAttachShader(program, fragment);

	glLinkProgram(program);

	glDeleteShader(vertex);
	glDeleteShader(fragment);

	return program;
}

GLuint Shader::create(const std::string &vertex_source, const std::string &fragment_source, const std::string& geometry_source) {
	auto vertex = compile(vertex_source, GL_VERTEX_SHADER);
	auto fragment = compile(fragment_source, GL_FRAGMENT_SHADER);
	GLuint geometry = compile(geometry_source, GL_GEOMETRY_SHADER);

	GLuint program = glCreateProgram();
	glAttachShader(program, vertex);
	glAttachShader(program, fragment);
	glAttachShader(program, geometry);

	glLinkProgram(program);

	glDeleteShader(vertex);
	glDeleteShader(fragment);
	glDeleteShader(geometry);

	return program;
}

GLuint Shader::load(const std::string &vertex_file_path, const std::string &fragment_file_path, const std::optional<std::string>& geometry_file_path) {
	if (geometry_file_path) {
		return create(source(vertex_file_path), source(fragment_file_path), source(geometry_file_path.value()));
	}

	return create(source(vertex_file_path), source(fragment_file_path));
}

void
Shader::preload(const std::string &name, const std::string &vertex_file_path, const std::string &fragment_file_path, const std::optional<std::string> &geometry_file_path) {
	shaders[name] = load(vertex_file_path, fragment_file_path, geometry_file_path);
}

