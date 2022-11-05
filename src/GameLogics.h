#pragma once

#include <vector>
#include <glm/glm.hpp>

class WorldObject {
public:
	glm::vec3 pos;
};

class Eatable {
public:
	virtual void consume(Snake snake) {};
};

class World {
public:
	std::vector<WorldObject> worldObjects;
};

class Snake {
public:
	std::vector<glm::vec3> segments;
	glm::vec3 motion;
	float length;
	
	void tick(double gameTime);
};