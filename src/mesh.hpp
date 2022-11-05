#pragma once

#include <vector>
#include <array>

#include <glm/glm.hpp>

using namespace glm;

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