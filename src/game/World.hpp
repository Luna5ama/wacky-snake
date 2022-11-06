#pragma once

#include <vector>
#include <random>

#include "Object.hpp"

struct World {
	std::vector<ItemObj> objects;

	std::random_device rd;
	std::default_random_engine re;

	World() :
		objects(), rd(), re(rd()) {}



	// returns nullptr on failure, ignores None items
	// type T must have distance function that takes an Object as input
	template <class T> [[nodiscard]]
	ItemObj* checkCollision(const T& obj, Object* filter = nullptr) {
		for (auto& object : objects) {
			if (object.item == Item::None || &object == filter) continue;

			if (obj.dist(object) <= 0.0) return &object;
		}

		return nullptr;
	}

	// collider is any object that implements collides(Object) aka. the snake
	template <class T>
	void moveObj(Object& obj, const T& collider) {
		std::uniform_int_distribution<int> dist(-63, 63);

		do {

			obj.pos = glm::vec3(dist(re), dist(re), dist(re));

		} while (checkCollision(obj, &obj) != nullptr || collider.collides(obj));
	}

	template <class T>
	void placeFood(const T& collider) {
		ItemObj food{ glm::vec3(0.0), 0.5, Item::Food };

		moveObj(food, collider);

		objects.emplace_back(std::move(food));
	}
};