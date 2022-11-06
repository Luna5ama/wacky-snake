#include "Snake.hpp"

Snake::Snake() :
	segments({ glm::vec3(0.0), glm::vec3(0.0, 0.0, -20.0) }),
	rotation(),
	length(20.0f) {}