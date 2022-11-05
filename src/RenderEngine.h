#pragma once

#include <glm/glm.hpp>

struct ProjViewModelMatrix {
	glm::mat4 projection;
	glm::mat4 view;
	glm::mat4 model;
};

class Camera {
public:
	ProjViewModelMatrix matrix;
	glm::vec2 rotation;
	float fov;

	void updateProjection(glm::vec2 windowSize);
	void updateView(glm::vec2 mousePosDelta);
};