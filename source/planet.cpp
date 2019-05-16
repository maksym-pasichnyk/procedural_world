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

#include <glm/ext/quaternion_geometric.hpp>
#include "perlin3d.h"
#include "planet.h"

/*struct Edge3  {
	const glm::vec3 A;
	const glm::vec3 B;
	const glm::vec3 Delta;

	Edge3(glm::vec3 a, glm::vec3 b) : A(a), B(b), Delta(b - a) {}

	glm::vec3 PointAt(float t) const {
		return A + t * Delta;
	}

	float LengthSquared() const {
		return glm::dot(Delta, Delta);
	}

	float Project(glm::vec3 p) const {
		return glm::dot(p - A, Delta) / LengthSquared();
	}
};

struct Triangle {
	const glm::vec3 A;
	const glm::vec3 B;
	const glm::vec3 C;

	const Edge3 EdgeAb;
	const Edge3 EdgeBc;
	const Edge3 EdgeCa;

	const glm::vec3 TriNorm;

	Triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c) : A(a), B(b), C(c), EdgeAb(a, b), EdgeBc(b, c), EdgeCa(c, a), TriNorm(glm::cross(a - b, a - c)) {

	}

	glm::vec3 ClosestPointTo(glm::vec3 p) {
		auto uab = EdgeAb.Project( p );
		auto uca = EdgeCa.Project( p );

		if (uca > 1 && uab < 0)
			return A;

		auto ubc = EdgeBc.Project( p );

		if (uab > 1 && ubc < 0)
			return B;

		if (ubc > 1 && uca < 0)
			return C;

		if (ZeroToOne.Contains( uab ) && !PlaneAb.IsAbove( p ))
			return EdgeAb.PointAt( uab );

		if (ZeroToOne.Contains( ubc ) && !PlaneBc.IsAbove( p ))
			return EdgeBc.PointAt( ubc );

		if (ZeroToOne.Contains( uca ) && !PlaneCa.IsAbove( p ))
			return EdgeCa.PointAt( uca );

		// The closest point is in the triangle so
		// project to the plane to find it
		return TriPlane.Project( p );

	}
};*/

glm::vec3 Planet::getPoint(const glm::vec3 &point) {
	glm::vec3 out;

	for (int i = 0; i < indices.size(); i += 3) {
		auto v0 = vertices[i];
		auto v1 = vertices[i + 1];
		auto v2 = vertices[i + 2];


	}
//	std::optional<float> min = std::nullopt;
//
//
//	for (auto const& p : vertices) {
//		auto d = p - point;
//		float dist = glm::dot(d, d);
//
//		if (!out || (dist < min)) {
//			min = dist;
//			out = p;
//		}
//	}

//	auto n_point = glm::normalize(point);
//	return n_point * (radius + static_cast<float>(perlin3d::noise(n_point, 8)) * height_variation);

	return out;
}

std::unique_ptr<GameObject> createPlanet(const glm::vec3& position, int level_of_detail, float radius, float height_variation) {
	glm::vec3 dirt{0.35f, 0.3f, 0.3f};

	std::vector<glm::vec3> vertices{};
	std::vector<glm::vec3> colors{};
	std::vector<glm::vec3> normals{};
	std::vector<uint32_t> indices{};

	glm::vec3 v[12] = {
			glm::vec3{ 0.00000000000000000000000000000000, -1.00000000000000000000000000000000,  0.00000000000000000000000000000000},
			glm::vec3{-0.85065118825597217565853486171265, -0.44721279658979876060686617594586, -0.27639332568829129259625477607987},
			glm::vec3{ 0.00000000000000000000000000000000, -0.44721279658979876060686617594586, -0.89442759045454947215395023797622},
			glm::vec3{ 0.85065118825597217565853486171265, -0.44721279658979876060686617594586, -0.27639332568829129259625477607987},
			glm::vec3{ 0.52573134691267619223154450033364, -0.44721279658979876060686617594586,  0.72360712091556602867322989506798},
			glm::vec3{-0.52573134691267619223154450033364, -0.44721279658979876060686617594586,  0.72360712091556602867322989506798},
			glm::vec3{-0.52573134691267619223154450033364,  0.44721279658979876060686617594586, -0.72360712091556602867322989506798},
			glm::vec3{ 0.52573134691267619223154450033364,  0.44721279658979876060686617594586, -0.72360712091556602867322989506798},
			glm::vec3{ 0.85065118825597217565853486171265,  0.44721279658979876060686617594586,  0.27639332568829129259625477607987},
			glm::vec3{ 0.00000000000000000000000000000000,  0.44721279658979876060686617594586,  0.89442759045454947215395023797622},
			glm::vec3{-0.85065118825597217565853486171265,  0.44721279658979876060686617594586,  0.27639332568829129259625477607987},
			glm::vec3{ 0.00000000000000000000000000000000,  1.00000000000000000000000000000000,  0.00000000000000000000000000000000}
	};

	auto truncate = [&](const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2) {
		auto p0 = glm::normalize(v0);
		auto p1 = glm::normalize(v1);
		auto p2 = glm::normalize(v2);

		auto normal = glm::normalize(glm::cross((p1 - p0), (p2 - p0)));

		auto baseIndex = static_cast<uint32_t>(vertices.size());

		auto h0 = static_cast<float>(perlin3d::noise(p0, 8));
		auto h1 = static_cast<float>(perlin3d::noise(p1, 8));
		auto h2 = static_cast<float>(perlin3d::noise(p2, 8));

		vertices.push_back(p0 * (radius + h0 * height_variation));
		vertices.push_back(p1 * (radius + h1 * height_variation));
		vertices.push_back(p2 * (radius + h2 * height_variation));

		normals.push_back(normal);
		normals.push_back(normal);
		normals.push_back(normal);

		colors.push_back(dirt * (h0 * 0.6f + 0.4f));
		colors.push_back(dirt * (h1 * 0.6f + 0.4f));
		colors.push_back(dirt * (h2 * 0.6f + 0.4f));

		indices.push_back(baseIndex);
		indices.push_back(baseIndex + 1);
		indices.push_back(baseIndex + 2);
	};

	auto subdivide = [&](const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2) {
		struct Triangle {
			glm::vec3 v0, v1, v2;
		};

		std::vector<Triangle> vertices{};
		vertices.push_back({ v0, v1, v2 });

		for (int i = 0; i < level_of_detail; i++) {
			std::vector<Triangle> new_vertices{};
			for (auto data : vertices) {
				auto m0 = (data.v0 + data.v1) * 0.5f;
				auto m1 = (data.v1 + data.v2) * 0.5f;
				auto m2 = (data.v2 + data.v0) * 0.5f;

				new_vertices.push_back({m2, data.v0, m0 });
				new_vertices.push_back({m0, data.v1, m1 });
				new_vertices.push_back({m1, data.v2, m2 });
				new_vertices.push_back({m0, m1, m2 });
			}
			vertices = new_vertices;
		}

		for (auto data : vertices) {
			truncate(data.v0, data.v1, data.v2);
		}
	};

	for (int i = 0; i < 5; i++) {
		subdivide(v[0], v[i + 1], v[(i + 1) % 5 + 1]);
		subdivide(v[i + 1], v[i + 6], v[(i + 1) % 5 + 1]);
		subdivide(v[(i + 1) % 5 + 1], v[i + 6], v[(i + 1) % 5 + 6]);
		subdivide(v[11], v[(i + 1) % 5 + 6], v[i + 6]);
	}

	Mesh mesh{};
	mesh.setColors(colors);
	mesh.setNormals(normals);
	mesh.setIndices(indices);
	mesh.setVertices(vertices);

	auto object = std::make_unique<Planet>(vertices, indices, level_of_detail, radius, height_variation);
	object->mesh = std::move(mesh);
	object->mesh.shader = Shader::find("default");
	object->transform.position = position;
	return std::move(object);
};