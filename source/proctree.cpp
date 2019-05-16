#include <cmath>
#include <string.h>
#include "proctree.h"
#include "mesh_builder.h"

#include <functional>

#include <iostream>

struct Branch {
	Branch *child_1 = nullptr;
	Branch *child_2 = nullptr;
	Branch *parent = nullptr;
	glm::vec3 head{};
	glm::vec3 tangent{};
	float length = 1;
	int trunktype = 0;
	std::vector<int> ring_1;
	std::vector<int> ring_2;
	std::vector<int> ring_3;
	std::vector<int> root_ring;
	float radius = 0;
	int last = 0;

	Branch() = default;
	Branch(glm::vec3 head, Branch *parent);

	~Branch();

	void split(int level, int aSteps, TreeProperties &properties, int aL1 = 1, int aL2 = 1);
};

static glm::vec3 mirrorBranch(glm::vec3 a, glm::vec3 normal, TreeProperties &aProperties);
static glm::vec3 scaleInDirection(glm::vec3 a, glm::vec3 b, float scale);

static glm::vec3 scaleInDirection(glm::vec3 a, glm::vec3 b, float scale) {
	return a + b * glm::dot(a, b) * (scale - 1);
}

static glm::vec3 vecAxisAngle(glm::vec3 a, glm::vec3 axis, float angle) {
	//v std::cos(T) + (axis x v) * std::sin(T) + axis*(axis . v)(1-std::cos(T)
	float cosr = std::cos(angle);
	float sinr = std::sin(angle);

	return a * cosr + glm::cross(axis, a) * sinr + axis * dot(axis, a) * (1 - cosr);
}

static glm::vec3 mirrorBranch(glm::vec3 a, glm::vec3 normal, TreeProperties &properties) {
	glm::vec3 v = glm::cross(normal, glm::cross(a, normal));
	return a - v * properties.branch_factor * glm::dot(v, a);
}

Branch::~Branch() {
	delete child_1;
	delete child_2;
}

Branch::Branch(glm::vec3 head, Branch *parent) : parent{parent}, head{head} {}

void Branch::split(int level, int aSteps, TreeProperties &properties, int aL1, int aL2) {
	int rLevel = properties.levels - level;
	glm::vec3 po{};
	if (parent) {
		po = parent->head;
	} else {
		trunktype = 1;
	}
	glm::vec3 so = head;
	glm::vec3 dir = glm::normalize(so - po);

	glm::vec3 normal = glm::cross(dir, {dir.z, dir.x, dir.y});
	glm::vec3 tangent = glm::cross(dir, normal);
	float r = properties.next(rLevel * 10 + aL1 * 5.0f + aL2 + properties.seed);
	//float r2 = am_properties.random(rLevel * 10 + aL1 * 5.0f + aL2 + 1 + am_properties.seed); // never used

	glm::vec3 adj = normal * r + tangent * (1 - r);
	if (r > 0.5) adj = -adj;

	float clump = (properties.clump_max - properties.clump_min) * r + properties.clump_min;
	glm::vec3 newdir = glm::normalize(adj * (1 - clump) + dir * clump);
	glm::vec3 newdir2 = mirrorBranch(newdir, dir, properties);

	if (r > 0.5f) {
		std::swap(newdir, newdir2);
	}

	if (aSteps > 0) {
		auto angle = static_cast<float>(aSteps / (float) properties.tree_steps * 2 * M_PI * properties.twist_rate);
		newdir2 = glm::normalize(glm::vec3{std::sin(angle), r, std::cos(angle)});
	}

	float growAmount = level * level / (float) (properties.levels * properties.levels) * properties.grow_amount;
	float dropAmount = rLevel * properties.drop_amount;
	float sweepAmount = rLevel * properties.sweep_amount;

	glm::vec3 a{sweepAmount, dropAmount + growAmount, 0};
	newdir = glm::normalize(newdir + a);
	newdir2 = glm::normalize(newdir2 +  a);

	glm::vec3 head0 = so + newdir * length;
	glm::vec3 head1 = so + newdir2 * length;
	child_1 = new Branch(head0, this);
	child_2 = new Branch(head1, this);
	child_1->length = std::pow(length, properties.length_falloff_power) * properties.length_falloff_factor;
	child_2->length = std::pow(length, properties.length_falloff_power) * properties.length_falloff_factor;

	if (level > 0) {
		if (aSteps > 0) {
			child_1->head = head + glm::vec3{(r - 0.5f) * 2 * properties.trunk_kink, properties.climb_rate, (r - 0.5f) * 2 * properties.trunk_kink};
			child_1->trunktype = 1;
			child_1->length = length * properties.taper_rate;
			child_1->split(level, aSteps - 1, properties, aL1 + 1, aL2);
		} else {
			child_1->split(level - 1, 0, properties, aL1 + 1, aL2);
		}
		child_2->split(level - 1, 0, properties, aL1, aL2 + 1);
	}
}

std::unique_ptr<GameObject> createProcTree(const glm::vec3& position, const std::optional<TreeProperties>& properties) {
	auto m_properties = properties.value_or(TreeProperties{});

	auto root = std::make_unique<Branch>(glm::vec3{0, m_properties.trunk_length, 0}, nullptr);
	m_properties.rseed = m_properties.seed;
	root->length = m_properties.initial_branch_length;
	root->split(m_properties.levels, m_properties.tree_steps, m_properties);

	size_t vertices_count = 0;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> colors;
	std::vector<glm::vec3> normals;
	std::vector<glm::ivec3> faces;

	std::function<void(Branch *)> calcVertSizes;
	calcVertSizes = [&](Branch *branch) {
		int segments = m_properties.segments;

		if (!branch->parent) {
			vertices_count += segments;
		}

		if (branch->child_1) {
			vertices_count += 1 + (segments / 2) - 1 + 1 + (segments / 2) - 1 + (segments / 2) - 1;

			calcVertSizes(branch->child_1);
			calcVertSizes(branch->child_2);
		} else {
			vertices_count++;
//			vertices_count += 8;
//			faces_count += 4;
		}
	};

	std::function<void(Branch *)> createTriangles;
	createTriangles = [&](Branch *branch) {
		int segments = m_properties.segments;

		if (!branch->parent) {
			auto tangent = glm::normalize(glm::cross(branch->child_1->head - branch->head, branch->child_2->head - branch->head));
			glm::vec3 left{-1, 0, 0};

			float angle = std::acos(glm::dot(tangent, left));
			if (glm::dot(glm::cross(left, tangent), glm::normalize(branch->head)) > 0) {
				angle = static_cast<float>(2 * M_PI - angle);
			}
			int segOffset = (int) floor(0.5f + (angle / M_PI / 2 * segments));
			for (int i = 0; i < segments; i++) {
				int v1 = branch->ring_1[i];
				int v2 = branch->root_ring[(i + segOffset + 1) % segments];
				int v3 = branch->root_ring[(i + segOffset) % segments];
				int v4 = branch->ring_1[(i + 1) % segments];

				faces.emplace_back(v1, v4, v3);
				faces.emplace_back(v4, v2, v3);
			}
		}

		if (!branch->child_1->ring_1.empty()) {
			int segOffset0 = -1, segOffset1 = -1;
			float match0 = 0, match1 = 0;

			auto v1 = glm::normalize(vertices[branch->ring_2[0]] - branch->head);
			auto v2 = glm::normalize(vertices[branch->ring_3[0]] - branch->head);

			v1 = scaleInDirection(v1, glm::normalize(branch->child_1->head - branch->head), 0);
			v2 = scaleInDirection(v2, glm::normalize(branch->child_2->head - branch->head), 0);

			for (int i = 0; i < segments; i++) {
				float l = glm::dot(glm::normalize(vertices[branch->child_1->ring_1[i]] - branch->child_1->head), v1);
				if (segOffset0 == -1 || l > match0) {
					match0 = l;
					segOffset0 = segments - i;
				}
				l = glm::dot(glm::normalize(vertices[branch->child_2->ring_1[i]] - branch->child_2->head), v2);
				if (segOffset1 == -1 || l > match1) {
					match1 = l;
					segOffset1 = segments - i;
				}
			}

			for (int i = 0; i < segments; i++) {
				int i1 = branch->child_1->ring_1[i];
				int i2 = branch->ring_2[(i + segOffset0 + 1) % segments];
				int i3 = branch->ring_2[(i + segOffset0) % segments];
				int i4 = branch->child_1->ring_1[(i + 1) % segments];

				faces.emplace_back(i1, i4, i3);
				faces.emplace_back(i4, i2, i3);

				i1 = branch->child_2->ring_1[i];
				i2 = branch->ring_3[(i + segOffset1 + 1) % segments];
				i3 = branch->ring_3[(i + segOffset1) % segments];
				i4 = branch->child_2->ring_1[(i + 1) % segments];

				faces.emplace_back(i1, i2, i3);
				faces.emplace_back(i1, i4, i2);
			}

			createTriangles(branch->child_1);
			createTriangles(branch->child_2);
		} else {
			for (int i = 0; i < segments; i++) {
				faces.emplace_back(branch->child_1->last, branch->ring_2[(i + 1) % segments], branch->ring_2[i]);
				faces.emplace_back(branch->child_2->last, branch->ring_3[(i + 1) % segments], branch->ring_3[i]);
			}
		}
	};

	std::function<void(Branch*, float)> createForks;
	createForks = [&](Branch *branch, float radius) {
		if (radius == 0) {
			radius = m_properties.max_radius;
		}

		branch->radius = radius;

		if (radius > branch->length) {
			radius = branch->length;
		}

		int segments = m_properties.segments;

		auto segmentAngle = static_cast<float>(M_PI * 2 / (float) segments);

		if (!branch->parent) {
			branch->root_ring.resize(static_cast<size_t>(segments));

			glm::vec3 axis = {0, 1, 0};
			glm::vec3 left = {-1, 0, 0};

			float r_radius = radius / m_properties.radius_falloff_rate;

			for (int i = 0; i < segments; i++) {
				branch->root_ring[i] = static_cast<int>(vertices_count);
				vertices[vertices_count++] = vecAxisAngle(left, axis, -segmentAngle * i) * r_radius;
			}
		}

		if (branch->child_1) {
			glm::vec3 axis;
			if (branch->parent) {
				axis = glm::normalize(branch->head - branch->parent->head);
			} else {
				axis = glm::normalize(branch->head);
			}

			auto axis1 = glm::normalize(branch->head - branch->child_1->head);
			auto axis2 = glm::normalize(branch->head - branch->child_2->head);
			auto tangent = glm::normalize(glm::cross(axis1, axis2));
			branch->tangent = tangent;

			auto axis3 = glm::normalize(glm::cross(tangent, glm::normalize(-axis1 - axis2)));
			glm::vec3 dir{axis2.x, 0, axis2.z};

			auto centerloc = branch->head - dir * m_properties.max_radius * 0.5f;

			branch->ring_1.resize(static_cast<size_t>(segments));
			branch->ring_2.resize(static_cast<size_t>(segments));
			branch->ring_3.resize(static_cast<size_t>(segments));

			int ring0count = 0;
			int ring1count = 0;
			int ring2count = 0;

			float scale = m_properties.radius_falloff_rate;

			if (branch->child_1->trunktype || branch->trunktype) {
				scale = 1.0f / m_properties.taper_rate;
			}

			//main segment ring
			int linch0 = static_cast<int>(vertices_count);
			branch->ring_1[ring0count++] = linch0;
			branch->ring_3[ring2count++] = linch0;
			vertices[vertices_count++] = centerloc + tangent * radius * scale;

			int start = static_cast<int>(vertices_count - 1);
			glm::vec3 d1 = vecAxisAngle(tangent, axis2, 1.57f);
			glm::vec3 d2 = normalize(cross(tangent, axis));
			float s = 1 / dot(d1, d2);
			for (int i = 1; i < segments / 2; i++) {
				glm::vec3 vec = vecAxisAngle(tangent, axis2, segmentAngle * i);
				branch->ring_1[ring0count++] = start + i;
				branch->ring_3[ring2count++] = start + i;
				vec = scaleInDirection(vec, d2, s);
				vertices[vertices_count++] = centerloc + vec * radius * scale;
			}
			int linch1 = static_cast<int>(vertices_count);
			branch->ring_1[ring0count++] = linch1;
			branch->ring_2[ring1count++] = linch1;
			vertices[vertices_count++] = centerloc - tangent * radius * scale;
			for (int i = segments / 2 + 1; i < segments; i++) {
				glm::vec3 vec = vecAxisAngle(tangent, axis1, segmentAngle * i);
				branch->ring_1[ring0count++] = static_cast<int>(vertices_count);
				branch->ring_2[ring1count++] = static_cast<int>(vertices_count);
				vertices[vertices_count++] = centerloc + vec * radius * scale;
			}
			branch->ring_2[ring1count++] = linch0;
			branch->ring_3[ring2count++] = linch1;
			start = static_cast<int>(vertices_count - 1);
			for (int i = 1; i < segments / 2; i++) {
				branch->ring_2[ring1count++] = start + i;
				branch->ring_3[ring2count++] = start + (segments / 2 - i);

				glm::vec3 vec = vecAxisAngle(tangent, axis3, segmentAngle * i);
				vertices[vertices_count++] = centerloc + vec * radius * scale;
			}

			//child radius is related to the brans direction and the length of the branch
			//float length0 = length(sub(branch->head, branch->child_1->head)); // never used
			//float length1 = length(sub(branch->head, branch->child_2->head)); // never used

			float radius0 = 1 * radius * m_properties.radius_falloff_rate;
			float radius1 = 1 * radius * m_properties.radius_falloff_rate;
			if (branch->child_1->trunktype) {
				radius0 = radius * m_properties.taper_rate;
			}
			createForks(branch->child_1, radius0);
			createForks(branch->child_2, radius1);
		} else {
			branch->last = static_cast<int>(vertices_count);
			vertices[vertices_count++] = (branch->head);
		}
	};

	std::function<void(Branch*)> createTwigs;
	createTwigs = [&](Branch* branch) -> void {
		if (!branch->child_1) {
			glm::vec3 tangent = glm::normalize(glm::cross(branch->parent->child_1->head - branch->parent->head, branch->parent->child_2->head - branch->parent->head));
			glm::vec3 binormal = glm::normalize(branch->head - branch->parent->head);
			//glm::vec3 normal = cross(tangent, binormal); //never used

			int vert1 = vertices.size();
			vertices.push_back(branch->head + tangent * m_properties.twig_scale + binormal * (m_properties.twig_scale * 2 - branch->length));
			int vert2 = vertices.size();
			vertices.push_back(branch->head - tangent * m_properties.twig_scale + binormal * (m_properties.twig_scale * 2 - branch->length));
			int vert3 = vertices.size();
			vertices.push_back(branch->head - tangent * m_properties.twig_scale - binormal * branch->length);
			int vert4 = vertices.size();
			vertices.push_back(branch->head + tangent * m_properties.twig_scale - binormal * branch->length);

			int vert8 = static_cast<int>(vertices.size());
			vertices.push_back(branch->head + tangent * m_properties.twig_scale + binormal * (m_properties.twig_scale * 2 - branch->length));
			int vert7 = static_cast<int>(vertices.size());
			vertices.push_back(branch->head - tangent * m_properties.twig_scale + binormal * (m_properties.twig_scale * 2 - branch->length));
			int vert6 = static_cast<int>(vertices.size());
			vertices.push_back(branch->head - tangent * m_properties.twig_scale - binormal * branch->length);
			int vert5 = static_cast<int>(vertices.size());
			vertices.push_back(branch->head + tangent * m_properties.twig_scale - binormal * branch->length);

			faces.emplace_back(vert1, vert2, vert3);
			faces.emplace_back(vert4, vert1, vert3);
			faces.emplace_back(vert6, vert7, vert8);
			faces.emplace_back(vert6, vert8, vert5);
		} else {
			createTwigs(branch->child_1);
			createTwigs(branch->child_2);
		}
	};

	vertices_count = 0;
	calcVertSizes(root.get());
	vertices.resize(vertices_count);

	colors.reserve(vertices_count);
	for (int i = 0; i < vertices_count; i++) {
		colors.emplace_back(81.0f / 255.0f, 56.0f / 255.0f, 56.0f / 255.0f);
	}


	vertices_count = 0;
	createForks(root.get(), 0);
	createTriangles(root.get());
	createTwigs(root.get());

	for (int i = colors.size(); i < vertices.size(); i++) {
		colors.emplace_back(0.0f / 255.0f, 200.0f / 255.0f, 0.0f / 255.0f);
	}

	int normalCount[vertices.size()];
	normals.resize(vertices.size());
	for (auto const& face : faces) {
		int i1 = face[0];
		int i2 = face[1];
		int i3 = face[2];

		auto normal = glm::normalize(glm::cross(vertices[i2] - vertices[i1], vertices[i3] - vertices[i1]));

		normals[i1] += normal;
		normals[i2] += normal;
		normals[i3] += normal;

		normalCount[i1]++;
		normalCount[i2]++;
		normalCount[i3]++;
	}
//
//	for (int i = 0; i < normals.size(); i++) {
//		normals[i] /= normalCount[i];
//	}

	MeshBuilder builder;
	builder.colors = colors;
	builder.vertices = vertices;
	builder.normals = normals;

	for (auto &face : faces) {
		builder.indices.emplace_back(face[0]);
		builder.indices.emplace_back(face[1]);
		builder.indices.emplace_back(face[2]);
	}

	auto object = std::make_unique<GameObject>();
	object->mesh = builder.build();
	object->mesh.shader = Shader::find("default_wood");
	object->transform.position = position;
	return std::move(object);
}