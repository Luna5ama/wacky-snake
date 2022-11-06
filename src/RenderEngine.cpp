#include "RenderEngine.h"
#include <GLFW/glfw3.h>
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include "Main.h"

#include <iostream>

Camera::Camera() : matrix(), rotation(), fov(90.0f) {}

void Camera::updateProjection(GameWindow& gameWindow) {
	this->matrix.projection = glm::perspective(this->fov, (float) gameWindow.windowSize.x / (float) gameWindow.windowSize.y, 0.001f, 1024.0f);
}
	
void Camera::updateView(glm::vec2 mousePosDelta) {
	float mouseSpeed = 0.1f;
	this->rotation += mousePosDelta * mouseSpeed;
	this->rotation.y = std::clamp(this->rotation.y, -90.0f, 90.0f);
	this->matrix.modelView = glm::identity<glm::mat4>();
	this->matrix.modelView = glm::rotate(this->matrix.modelView, glm::radians(this->rotation.x), glm::vec3(0.0f, 1.0f, 0.0f));
	this->matrix.modelView = glm::rotate(this->matrix.modelView, glm::radians(this->rotation.y), glm::vec3(1.0f, 0.0f, 0.0f));
}

SkyboxRenderer::SkyboxRenderer(RenderEngine& renderEngine): renderEngine(renderEngine), borderShaderProgram("resources/shaders/Border.vert.glsl", "resources/shaders/Border.frag.glsl") {	
	glm::vec3 borderVertices[] = {
		// Down
		{ 128.0f, -128.0f, -128.0f },
		{ -128.0f, -128.0f, -128.0f },
		{ 128.0f, -128.0f, 128.0f},
		{ -128.0f, -128.0f, 128.0f },
		{ -128.0f, -128.0f, -128.0f },
		{ 128.0f, -128.0f, 128.0f },
		// Up
		{ 128.0f, 128.0f, -128.0f },
		{ -128.0f, 128.0f, -128.0f },
		{ 128.0f, 128.0f, 128.0f },
		{ -128.0f, 128.0f, 128.0f },
		{ -128.0f, 128.0f, -128.0f },
		{ 128.0f, 128.0f, 128.0f },
		// West
		{ -128.0f, -128.0f, -128.0f },
		{ -128.0f, -128.0f, 128.0f },
		{ -128.0f, 128.0f, -128.0f },
		{ -128.0f, 128.0f, 128.0f },
		{ -128.0f, -128.0f, 128.0f },
		{ -128.0f, 128.0f, -128.0f },
		// East
		{ 128.0f, -128.0f, -128.0f },
		{ 128.0f, -128.0f, 128.0f },
		{128.0f, 128.0f, -128.0f },
		{ 128.0f, 128.0f, 128.0f },
		{ 128.0f, -128.0f, 128.0f },
		{ 128.0f, 128.0f, -128.0f },
		// South
		{ -128.0f, -128.0f, -128.0f },
		{ 128.0f, -128.0f, -128.0f },
		{ -128.0f, 128.0f, -128.0f },
		{ 128.0f, 128.0f, -128.0f },
		{ 128.0f, -128.0f, -128.0f },
		{ -128.0f, 128.0f, -128.0f },
		// North
		{ -128.0f, -128.0f, 128.0f },
		{ 128.0f, -128.0f, 128.0f },
		{ -128.0f, 128.0f, 128.0f },
		{ 128.0f, 128.0f, 128.0f },
		{ 128.0f, -128.0f, 128.0f },
		{ -128.0f, 128.0f, 128.0f }
	};
	this->borderVBO.allocate(&borderVertices, sizeof(borderVertices), 0);
	this->borderVAO.attachVertexBuffer(
		this->borderVBO,
		OpenGL::VertexAttribute::Builder(12)
		.addFloat(0, 3, GL_FLOAT, false)
		.build()
	);
}

void SkyboxRenderer::render(GameWindow& gameWindow) {
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	this->borderShaderProgram.bind();
	this->borderShaderProgram.bindBuffer(GL_UNIFORM_BUFFER, this->renderEngine.globalUBO, "Global");
	this->borderVAO.bind();
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

RenderEngine::RenderEngine() : skyboxRenderer(*this) {
	globalUBO.allocate(268, GL_DYNAMIC_STORAGE_BIT);
}

void RenderEngine::setup(GameWindow& gameWindow, float tickDelta) {
	char mem[268];
	size_t i = 0;
	memcpy(mem + i, &this->camera.matrix, sizeof(ProjViewModelMatrix));
	i += sizeof(ProjViewModelMatrix);
	
	glm::mat4 inverseProjectionMatrix = glm::inverse(this->camera.matrix.projection);
	glm::mat4 inverseModelViewMatrix = glm::inverse(this->camera.matrix.modelView);
	memcpy(mem + i, &inverseProjectionMatrix, sizeof(glm::mat4));
	i += sizeof(glm::mat4);
	memcpy(mem + i, &inverseModelViewMatrix, sizeof(glm::mat4));
	i += sizeof(glm::mat4);
	glm::f32vec2 windowSize((float) gameWindow.windowSize.x, (float) gameWindow.windowSize.y);
	memcpy(mem + i, &windowSize, sizeof(glm::vec2));
	i += sizeof(glm::vec2);

	mem[i] = tickDelta;

	this->globalUBO.invalidate();
	glNamedBufferSubData(this->globalUBO.id, 0, 268, mem);
}