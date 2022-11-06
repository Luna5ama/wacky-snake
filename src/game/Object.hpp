#pragma once

#include <glm/glm.hpp>

struct Object {
	glm::vec3 pos;
	float radius;

	float dist(Object other) const {
		return glm::distance(pos, other.pos) - radius - other.radius;
	}
};

enum class Item {
	Food,

	None
};

struct ItemObj : public Object {
	Item item;
};