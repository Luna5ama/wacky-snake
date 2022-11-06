#include "RenderEngine.h"
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

Camera::Camera() : fov(90.0f) {

}

void Camera::updateProjection(glm::vec2 windowSize) {
	this->matrix.projection = glm::perspective(this->fov, windowSize.x / windowSize.y, 0.01f, 1024.0f);
}

void Camera::updateView(glm::vec2 mousePosDelta) {
	float mouseSpeed = 0.1f;
	this->rotation += mousePosDelta * mouseSpeed;
	this->rotation.y = std::clamp(this->rotation.y, -90.0f, 90.0f);
}

SkyboxShaderProgram::SkyboxShaderProgram() : OpenGL::ShaderProgram("resources/shaders/Background.vert.glsl", "resources/shaders/Background.frag.glsl") {
	this->mouseUniform = glGetUniformLocation(this->id, "mouse");
}

SkyboxRenderer::SkyboxRenderer(RenderEngine& renderEngine): renderEngine(renderEngine), borderShaderProgram("resources/shaders/Border.vert.glsl", "resources/shaders/Border.frag.glsl") {
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

void SkyboxRenderer::render() {
	this->skyboxShaderProgram.bind();
	this->skyboxShaderProgram.bindBuffer(GL_UNIFORM_BUFFER, this->renderEngine.globalUBO, "Global");
	this->skyboxVAO.bind();
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//this->borderShaderProgram.bind();
	//this->borderVAO.bind();
	//glDrawArrays(GL_TRIANGLES, 0, 36);
}

RenderEngine::RenderEngine() : skyboxRenderer(*this) {
	globalUBO.allocate(268, GL_DYNAMIC_STORAGE_BIT);
}

void RenderEngine::setupGlobalUBO(glm::vec2 windowSize, float tickDelta) {
	char* mem = new char[268];
	size_t i = 0;
	memccpy(mem + i, &this->camera.matrix, i, sizeof(ProjViewModelMatrix));
	i += sizeof(ProjViewModelMatrix);
	
	glm::mat4 inverseProjectionMatrix = glm::inverse(this->camera.matrix.projection);
	glm::mat4 inverseModelViewMatrix = glm::inverse(this->camera.matrix.modelView);
	memcpy(mem + i, &inverseProjectionMatrix, sizeof(glm::mat4));
	i += sizeof(glm::mat4);
	memcpy(mem + i, &inverseModelViewMatrix, sizeof(glm::mat4));
	i += sizeof(glm::mat4);
	memcpy(mem + i, &windowSize, sizeof(glm::vec2));
	i += sizeof(glm::vec2);

	mem[i] = tickDelta;

	this->globalUBO.invalidate();
	glNamedBufferSubData(this->globalUBO.id, 0, 268, mem);
	
	delete[] mem;
}