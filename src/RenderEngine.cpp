#include "RenderEngine.h"
#include <GL/glew.h>
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

SkyboxShaderProgram::SkyboxShaderProgram() : OpenGL::ShaderProgram("resources/shaders/Background.vert.glsl", "resources/shaders/Background.frag.glsl") {
	this->resolutionUniform = glGetUniformLocation(this->id, "resolution");
	this->mouseUniform = glGetUniformLocation(this->id, "mouse");
}

SkyboxRenderer::SkyboxRenderer() : borderShaderProgram("resources/shaders/Border.vert.glsl", "resources/shaders/Border.frag.glsl") {
	glm::vec2 shaderDrawVertices[] = {
		{ -1.0f, -1.0f },
		{ 1.0f, -1.0f },
		{ -1.0f, 1.0f },
		{ 1.0f, 1.0f },
		{ -1.0f, 1.0f },
		{ 1.0f, -1.0f }
	};
	this->skyboxVBO.allocate(&shaderDrawVertices, sizeof(shaderDrawVertices), 0);
	this->skyboxVAO.attachVertexBuffer(
		this->skyboxVBO,
		OpenGL::VertexAttribute::Builder(8)
		.addFloat(0, 2, GL_FLOAT, false)
		.build()
	);
	
	glm::vec3 borderDrawVertices[] = {
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
		{ 128.0f, 128.0f, -128.0f },
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

	
}

void SkyboxRenderer::render() const {
	this->skyboxShaderProgram.bind();
	this->skyboxVAO.bind();
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	this->borderShaderProgram.bind();
	this->borderVAO.bind();
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void RenderEngine::renderSkybox() {

}