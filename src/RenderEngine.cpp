#include "RenderEngine.h"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

void Camera::updateProjection(glm::vec2 windowSize) {
	this->matrix.projection = glm::perspective(this->fov, windowSize.x / windowSize.y, 0.01f, 1024.0f);
}

void Camera::updateView(glm::vec2 mousePosDelta) {
	float mouseSpeed = 0.1f;
	this->rotation += mousePosDelta * mouseSpeed;
	this->rotation.y = std::clamp(this->rotation.y, -90.0f, 90.0f);
}
