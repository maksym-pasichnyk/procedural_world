#[[
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
]]

cmake_minimum_required(VERSION 3.12)
project(world)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_executable(world source/main.cpp source/window.cpp include/window.h include/timer.h include/mesh.h source/mesh.cpp include/shader.h source/shader.cpp source/mesh_builder.cpp include/mesh_builder.h source/transform.cpp include/transform.h source/camera.cpp include/camera.h source/perlin3d.cpp include/perlin3d.h include/planet.h source/planet.cpp include/gameobject.h include/input.h include/module.h source/module.cpp source/input.cpp include/tree.h source/tree.cpp source/lsystem.cpp include/lsystem.h)

target_link_libraries(world glfw GL GLEW)
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})