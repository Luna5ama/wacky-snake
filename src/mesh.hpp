#pragma once

#include <vector>
#include <array>
#include <glm/glm.hpp>
#include "RenderEngine.hpp"

template <size_t N> [[nodiscard]]
std::array<glm::vec3, N / 3> createNormals(const std::array<glm::vec3, N>& mesh) {

	std::array<glm::vec3, N / 3> out{};

	for (int i = 0; i < N / 3; ++i) {
		glm::vec3 a = mesh[i * 3];
		glm::vec3 b = mesh[i * 3 + 1];
		glm::vec3 c = mesh[i * 3 + 2];

		out[i] = glm::normalize(glm::cross(b - a, c - a));
	}

	return out;
}

template <class T> [[nodiscard]]
std::vector<glm::vec3> createNormals(const T& mesh) {

	std::vector<glm::vec3> out;
	out.reserve(mesh.size() / 3);

	for (int i = 0; i < mesh.size() / 3; ++i) {
		glm::vec3 A = mesh[i * 3];
		glm::vec3 B = mesh[i * 3 + 1];
		glm::vec3 C = mesh[i * 3 + 2];

		out.emplace_back(glm::normalize(glm::cross(B - A, C - A)));
	}

	return out;
}

//creates an axis-aligned rectangular prism given two points
[[nodiscard]]
constexpr std::array<glm::vec3, 36> createRectMesh(const glm::vec3& p1, const glm::vec3& p2, float sidelen) {
	float halflen = sidelen * 0.5f;

	float minX = glm::min(p1.x, p2.x) - halflen;
	float minY = glm::min(p1.y, p2.y) - halflen;
	float minZ = glm::min(p1.z, p2.z) - halflen;

	float maxX = glm::max(p1.x, p2.x) + halflen;
	float maxY = glm::max(p1.y, p2.y) + halflen;
	float maxZ = glm::max(p1.z, p2.z) + halflen;

	glm::vec3 vertA(minX, minY, minZ);
	glm::vec3 vertB(maxX, minY, minZ);
	glm::vec3 vertC(minX, maxY, minZ);
	glm::vec3 vertD(maxX, maxY, minZ);

	glm::vec3 vertG(minX, minY, maxZ);
	glm::vec3 vertH(maxX, minY, maxZ);
	glm::vec3 vertE(minX, maxY, maxZ);
	glm::vec3 vertF(maxX, maxY, maxZ);

	return {
		vertA, vertD, vertB,
		vertA, vertC, vertD,

		vertB, vertF, vertH,
		vertB, vertD, vertF,

		vertC, vertF, vertD,
		vertC, vertE, vertF,

		vertA, vertB, vertH,
		vertA, vertH, vertG,

		vertA, vertG, vertE,
		vertA, vertE, vertC,

		vertG, vertH, vertF,
		vertG, vertF, vertE
	};
}

[[nodiscard]]
std::vector<glm::vec3> createSnakeMesh(const std::vector<glm::vec3>& points, float sidelen) {
	if (points.size() < 2) return {};

	std::vector<glm::vec3> out;
	out.reserve(points.size() * 36);

	for (int i = 1; i < points.size(); ++i) {
		auto p1 = points[i - 1];
		auto p2 = points[i];

		glm::vec3 dir = normalize(p2 - p1);
		glm::vec3 endp = p2;

		auto verts = createRectMesh(p1, endp, sidelen);
		for (auto&& vert : verts) {
			out.emplace_back(vert);
		}
	}

	return out;
}

void fillSnakeMeshInterleaved(const std::vector<glm::vec3>& points, PersistentMappedBuffer& buffer, float sidelen, glm::vec4 color) {
	auto snakeMesh = createSnakeMesh(points, sidelen);
	auto normals = createNormals(snakeMesh);

	for (int i = 0; i < snakeMesh.size(); i += 3) {
		int nIndex = i / 3;

		auto& curA = snakeMesh[i];
		memcpy(buffer.pointer + buffer.size, &curA, sizeof(glm::vec3));
		buffer.size += sizeof(glm::vec3);
		memcpy(buffer.pointer + buffer.size, &normals[nIndex], sizeof(glm::vec3));
		buffer.size += sizeof(glm::vec3);
		memcpy(buffer.pointer + buffer.size, &color, sizeof(glm::vec4));
		buffer.size += sizeof(glm::vec4);

		auto& curB = snakeMesh[i + 1];
		memcpy(buffer.pointer + buffer.size, &curB, sizeof(glm::vec3));
		buffer.size += sizeof(glm::vec3);
		memcpy(buffer.pointer + buffer.size, &normals[nIndex], sizeof(glm::vec3));
		buffer.size += sizeof(glm::vec3);
		memcpy(buffer.pointer + buffer.size, &color, sizeof(glm::vec4));
		buffer.size += sizeof(glm::vec4);

		auto& curC = snakeMesh[i + 2];
		memcpy(buffer.pointer + buffer.size, &curC, sizeof(glm::vec3));
		buffer.size += sizeof(glm::vec3);
		memcpy(buffer.pointer + buffer.size, &normals[nIndex], sizeof(glm::vec3));
		buffer.size += sizeof(glm::vec3);
		memcpy(buffer.pointer + buffer.size, &color, sizeof(glm::vec4));
		buffer.size += sizeof(glm::vec4);
	}
}

[[nodiscard]]
consteval std::array<glm::vec3, 20 * 3> createFoodMesh() {
	constexpr float a = 0.52573f;
	constexpr float b = 0.85065f;

	// adapted from https://github.com/anishagartia/Icosahedron_OpenGL
	constexpr std::array<glm::vec3, 12> verts = {
		glm::vec3{ -a, 0.0f, b }, { a, 0.0, b }, { -a, 0.0, -b }, { a, 0.0, -b },
		{ 0.0, b, a }, { 0.0, b, -a }, { 0.0, -b, a }, { 0.0, -b, -a },
		{ b, a, 0.0 }, { -b, a, 0.0 }, { b, -a, 0.0 }, { -b, -a, 0.0 }
	};

	//unit isocahedron
	constexpr std::array<glm::vec3, 20 * 3> isocahedronMesh = {
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

	return isocahedronMesh;
}

[[nodiscard]]
constexpr std::array<glm::vec3, 20 * 3> createFoodMesh(glm::vec3 pos, float r) {
	constexpr std::array<glm::vec3, 20*3> isocahedronMesh = createFoodMesh();

	std::array<glm::vec3, 20 * 3> out{};
	for (int i = 0; i < out.size(); ++i) {
		out[i] = isocahedronMesh[i] * r + pos;
	}

	return out;
}

void fillFoodMeshInterleaved(PersistentMappedBuffer& buffer, const glm::vec3& pos, float radius, const glm::vec4& color) {
	static constexpr auto isocahedronMesh = createFoodMesh();

	static auto normals = createNormals(isocahedronMesh);

	for (int i = 0; i < 20; i++) {
		glm::vec3 transformed = isocahedronMesh[i * 3] * radius + pos;
		memcpy(buffer.pointer + buffer.size, &transformed, sizeof(glm::vec3));
		buffer.size += sizeof(glm::vec3);
		memcpy(buffer.pointer + buffer.size, &normals[i], sizeof(glm::vec3));
		buffer.size += sizeof(glm::vec3);
		memcpy(buffer.pointer + buffer.size, &color, sizeof(glm::vec4));
		buffer.size += sizeof(glm::vec4);

		transformed = isocahedronMesh[i * 3 + 1] * radius + pos;
		memcpy(buffer.pointer + buffer.size, &transformed, sizeof(glm::vec3));
		buffer.size += sizeof(glm::vec3);
		memcpy(buffer.pointer + buffer.size, &normals[i], sizeof(glm::vec3));
		buffer.size += sizeof(glm::vec3);
		memcpy(buffer.pointer + buffer.size, &color, sizeof(glm::vec4));
		buffer.size += sizeof(glm::vec4);

		transformed = isocahedronMesh[i * 3 + 2] * radius + pos;
		memcpy(buffer.pointer + buffer.size, &transformed, sizeof(glm::vec3));
		buffer.size += sizeof(glm::vec3);
		memcpy(buffer.pointer + buffer.size, &normals[i], sizeof(glm::vec3));
		buffer.size += sizeof(glm::vec3);
		memcpy(buffer.pointer + buffer.size, &color, sizeof(glm::vec4));
		buffer.size += sizeof(glm::vec4);
	}
}

