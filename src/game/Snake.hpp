#pragma once

#include <vector>
#include <numbers>
#include <glm/glm.hpp>

#include "Object.hpp"
#include "World.hpp"

enum class LoseCode : unsigned char {
	None, // didn't lose
	Shrunk,	//shrunk out of existence
	Walled, //hit wall
	Snaked, //hit self
};

class Snake {
private:
	// flag preventing multiple turn segments being created per frame.
	bool turned = false;
	double timeSinceTurn = 100000.0;

protected:
	glm::vec3 direction;
	float length;

public:
	std::vector<glm::vec3> segments;
	size_t foodsEaten = 0;

	static constexpr float radius = 0.5f;
	static constexpr float speed = 1.0f; // speed in m/s
	//static constexpr float shrinkage = 0.926118f;
	static constexpr float shrinkage = 1.0f;

	Snake() :
		segments({ glm::vec3(0.0, -5.0, 0.0), glm::vec3(0.0, -5.0, -10.0) }),
		direction(glm::vec3(0.0, 0.0, 1.0)),
		length(10.0f) {}

	void setDirection(glm::vec3 dir) {
		// no change or change is impossible
		if (dir != direction && dir != -direction && timeSinceTurn*speed > radius && segments.size() > 0) {
			if (!turned) {
				segments.insert(segments.begin(), segments[0]);
				timeSinceTurn = 0.0;
				turned = true;
			}

			direction = dir;
		}
	}

	[[nodiscard]]
	constexpr float dist(Object obj, int offset = 0) const {

		// really big number
		float current = 9999999999999999999.0f;

		// https://iquilezles.org/articles/distfunctions/
		auto dist = [&](glm::vec3 s1, glm::vec3 s2) {
			glm::vec3 pa = obj.pos - s1, ba = s2 - s1;

			float h = glm::dot(pa, ba) / glm::dot(ba, ba);

			// clamp 0.0 - 1.0
			if (h < 0.0) h = 0.0;
			if (h > 1.0) h = 1.0;

			return glm::length(pa - ba * h) - obj.radius;
		};

		// a single segment snake should not exist
		for (int i = 1 + offset; i < segments.size(); ++i) {
			current = glm::min(current, dist(segments[i - 1], segments[i]));
		}

		return current;
	}

	// this isn't accurate to the model at all!
	[[nodiscard]]
	constexpr bool collides(Object obj) const {

		// ignore the lack of short curcuit eval
		return dist(obj) <= 0.0;
	}

	// shrinks snake by len meters
	void shrinkLen(float len) {
		while (len > 0.0f && segments.size() > 1) {
			auto& cur = segments.back();
			const auto& prev = *(segments.end() - 2);

			auto dir = prev - cur;

			float mLen = glm::length(dir);

			if (len > mLen) {
				segments.pop_back();
				len -= mLen;
			}
			else {
				glm::vec3 ndir = glm::normalize(dir);
				cur += ndir * len;
				len = 0.0f;
			}
		}

		if (len > 0.0f) [[unlikely]] {
			segments = {};
			length = 0.0f;
		}
		
	}

	// shinks snake based on delta time
	void shrink(float dt) {
		using namespace std::numbers;

		float newLength = length * glm::pow(shrinkage, dt);

		if (newLength <= radius) {
			segments = {}; //you just died!
			length = 0.0f;
		}
		else {
			float dLength = length - newLength;

			length = newLength;

			shrinkLen(dLength);
		}
	}

	// grows the snake by 1 meter via tail
	void grow() {
		length += 1;

		auto& back = segments.back();
		const auto& prev = *(segments.end() - 2);

		glm::vec3 ndir = glm::normalize(back - prev);
		back += ndir; // ignore that this can be abused to have the tail go out of bounds
	}

	// returns a LoseCode other than None on game end
	[[nodiscard]] 
	LoseCode tick(float dt, World& world) {
		timeSinceTurn += dt;
		turned = false;
		auto& head = segments[0];

		glm::vec3 target = head + speed * direction * dt;
		Object bounding{ target, radius };

		if (glm::abs(target.x) > 128.0f ||
			glm::abs(target.y) > 128.0f ||
			glm::abs(target.z) > 128.0f) {

			// snake out of bounds :(
			return LoseCode::Walled;
		}

		if (dist(bounding, 2) <= 0.0) {
			// snake collided with self :(
			return LoseCode::Snaked;
		}

		//@TODO deal with large, rapid, and evil dt changes

		auto collided = world.checkCollision(bounding);

		if (collided != nullptr) {
			switch (collided->item)
			{
			case Item::Food:
				grow();
				world.moveObj(*collided, *this);
				foodsEaten += 1;
				break;
			default:
				break;
			}
		}

		shrink(dt);

		if (segments.size() < 2) {
			// shrunk or something :(
			return LoseCode::Shrunk;
		}

		head = target;
		shrinkLen(speed * dt);
		return LoseCode::None;
	};
};