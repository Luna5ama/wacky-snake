#pragma once

#include <vector>
#include <array>

#include <glm/glm.hpp>
#include "RenderEngine.h"

using namespace glm;

template <size_t N> [[nodiscard]]
std::array<vec3, N / 3> createNormals(const std::array<vec3, N>& mesh) {

	std::array<vec3, N / 3> out{};

	for (int i = 0; i < N / 3; ++i) {
		vec3 A = mesh[i * 3];
		vec3 B = mesh[i * 3 + 1];
		vec3 C = mesh[i * 3 + 2];

		out[i] = glm::normalize(glm::cross(B - A, C - A)));
	}

	return out;
}

template <class T> [[nodiscard]]
std::vector<vec3> createNormals(const T& mesh) {

	std::vector<vec3> out;
	out.reserve(mesh.size() / 3);

	for (int i = 0; i < mesh.size() / 3; ++i) {
		vec3 A = mesh[i * 3];
		vec3 B = mesh[i * 3 + 1];
		vec3 C = mesh[i * 3 + 2];

		out.emplace_back(glm::normalize(glm::cross(B - A, C - A)));
	}

	return out;
}

//creates an axis-aligned rectangular prism given two points
[[nodiscard]]
constexpr std::array<vec3, 36> createRectMesh(const vec3& p1, const vec3& p2, float sidelen) {
	float halflen = sidelen * 0.5f;

	float minX = min(p1.x, p2.x) - halflen;
	float minY = min(p1.y, p2.y) - halflen;
	float minZ = min(p1.z, p2.z) - halflen;

	float maxX = max(p1.x, p2.x) + halflen;
	float maxY = max(p1.y, p2.y) + halflen;
	float maxZ = max(p1.z, p2.z) + halflen;

	vec3 vertA(minX, minY, minZ);
	vec3 vertB(maxX, minY, minZ);
	vec3 vertC(minX, maxY, minZ);
	vec3 vertD(maxX, maxY, minZ);

	vec3 vertG(minX, minY, maxZ);
	vec3 vertH(maxX, minY, maxZ);
	vec3 vertE(minX, maxY, maxZ);
	vec3 vertF(maxX, maxY, maxZ);

	return {
		vertA, vertB, vertD,
		vertA, vertD, vertC,

		vertB, vertH, vertF,
		vertB, vertF, vertD,

		vertC, vertD, vertF,
		vertC, vertF, vertE,

		vertA, vertH, vertB,
		vertA, vertG, vertH,

		vertA, vertE, vertG,
		vertA, vertC, vertE,

		vertG, vertF, vertH,
		vertG, vertE, vertF
	};
}

[[nodiscard]]
std::vector<vec3> createSnakeMesh(const std::vector<vec3>& points, float sidelen) {
	std::vector<vec3> out;
	out.reserve(points.size() * 36);

	for (int i = 1; i < points.size() - 1; ++i) {
		auto p1 = points[i - 1];
		auto p2 = points[i];

		vec3 dir = normalize(p2 - p1);

		vec3 endp = p2 - dir * sidelen;

		auto verts = createRectMesh(p1, endp, sidelen);
		for (auto&& vert : verts) {
			out.emplace_back(vert);
		}
	}

	auto verts = createRectMesh(points.back(), points.back(), sidelen);
	for (auto&& vert : verts) {
		out.emplace_back(vert);
	}

	return out;
}

constexpr void fillFoodMeshInterleaved(PersistentMappedBuffer& buffer, vec3 pos, float raidius) {
	constexpr float a = 0.52573;
	constexpr float b = 0.85065;

	// adapted from https://github.com/anishagartia/Icosahedron_OpenGL
	constexpr std::array < vec3, 12> verts = {
		vec3(-a, 0.0, b), vec3(a, 0.0, b), vec3(-a, 0.0, -b), vec3(a, 0.0, -b),
		vec3(0.0, b, a), vec3(0.0, b, -a), vec3(0.0, -b, a), vec3(0.0, -b, -a),
		vec3(b, a, 0.0), vec3(-b, a, 0.0), vec3(b, -a, 0.0), vec3(-b, -a, 0.0)
	};

	//unit isocahedron
	constexpr std::array<vec3, 20 * 3> isocahedronMesh = {
		verts[0], verts[4], verts[1],
		verts[0], verts[9], verts[4],
		verts[9], verts[5], verts[4],
		verts[4], verts[5], verts[8],
		verts[4], verts[8], verts[1],
		verts[8], verts[10], verts[1],
		verts[8], verts[3], verts[10],
		verts[5], verts[3], verts[8],
		verts[5], verts[2], verts[3],
		verts[2], verts[7], verts[3],
		verts[7], verts[10], verts[3],
		verts[7], verts[6], verts[10],
		verts[7], verts[11], verts[6],
		verts[11], verts[0], verts[6],
		verts[0], verts[1], verts[6],
		verts[6], verts[1], verts[10],
		verts[9], verts[0], verts[11],
		verts[9], verts[11], verts[2],
		verts[9], verts[2], verts[5],
		verts[7], verts[2], verts[11],
	};

	constexpr auto normals = createNormals(isocahedronMesh);


	for (int i = 0; i < 20; i++) {
		memcpy(buffer.pointer + buffer.size, &isocahedronMesh[i * 3], sizeof(vec3));
		buffer.size += sizeof(vec3);
		memcpy(buffer.pointer + buffer.size, &normals[i], sizeof(vec3));
		buffer.size += sizeof(vec3);
		
		memcpy(buffer.pointer + buffer.size, &isocahedronMesh[i * 3 + 1], sizeof(vec3));
		buffer.size += sizeof(vec3);
		memcpy(buffer.pointer + buffer.size, &normals[i], sizeof(vec3));
		buffer.size += sizeof(vec3);
		
		memcpy(buffer.pointer + buffer.size, &isocahedronMesh[i * 3 + 2], sizeof(vec3));
		buffer.size += sizeof(vec3);
		memcpy(buffer.pointer + buffer.size, &normals[i], sizeof(vec3));
		buffer.size += sizeof(vec3);
	}
}

[[nodiscard]]
constexpr std::array<vec3, 20 * 3> createFoodMesh(vec3 pos, float r) {
	constexpr float a = 0.52573;
	constexpr float b = 0.85065;

	// adapted from https://github.com/anishagartia/Icosahedron_OpenGL
	constexpr std::array < vec3, 12> verts = {
		vec3(-a, 0.0, b), vec3(a, 0.0, b), vec3(-a, 0.0, -b), vec3(a, 0.0, -b),
		vec3(0.0, b, a), vec3(0.0, b, -a), vec3(0.0, -b, a), vec3(0.0, -b, -a),
		vec3(b, a, 0.0), vec3(-b, a, 0.0), vec3(b, -a, 0.0), vec3(-b, -a, 0.0)
	};

	//unit isocahedron
	constexpr std::array<vec3, 20 * 3> isocahedronMesh = {
		verts[0], verts[4], verts[1],
		verts[0], verts[9], verts[4],
		verts[9], verts[5], verts[4],
		verts[4], verts[5], verts[8],
		verts[4], verts[8], verts[1],
		verts[8], verts[10], verts[1],
		verts[8], verts[3], verts[10],
		verts[5], verts[3], verts[8],
		verts[5], verts[2], verts[3],
		verts[2], verts[7], verts[3],
		verts[7], verts[10], verts[3],
		verts[7], verts[6], verts[10],
		verts[7], verts[11], verts[6],
		verts[11], verts[0], verts[6],
		verts[0], verts[1], verts[6],
		verts[6], verts[1], verts[10],
		verts[9], verts[0], verts[11],
		verts[9], verts[11], verts[2],
		verts[9], verts[2], verts[5],
		verts[7], verts[2], verts[11],
	};

	std::array<vec3, 20 * 3> out{};
	for (int i = 0; i < out.size(); ++i) {
		out[i] = isocahedronMesh[i] * r + pos;
	}

	return out;
}