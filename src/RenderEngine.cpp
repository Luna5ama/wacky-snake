#include "RenderEngine.h"
#include "Mesh.hpp"
#include <GLFW/glfw3.h>
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include "Main.h"
#include <iostream>

Camera::Camera() : matrix(), rotation(135.0f, 45.0f), fov(60.0f) {}

void Camera::updateProjection(GameWindow& gameWindow) {
	this->matrix.projection = glm::perspective(glm::radians(this->fov), (float) gameWindow.windowSize.x / (float) gameWindow.windowSize.y, 0.001f, 1024.0f);
}
	
void Camera::updateModelView(Game& game, glm::vec2 mousePosDelta) {
	float mouseSpeed = 0.1f;
	this->rotation += mousePosDelta * mouseSpeed;
	this->rotation.y = std::clamp(this->rotation.y, -90.0f, 90.0f);
	this->matrix.modelView = glm::identity<glm::mat4>();
	this->matrix.modelView = glm::rotate(this->matrix.modelView, glm::radians(this->rotation.y), glm::vec3(1.0f, 0.0f, 0.0f));
	this->matrix.modelView = glm::rotate(this->matrix.modelView, glm::radians(this->rotation.x), glm::vec3(0.0f, 1.0f, 0.0f));
	this->matrix.modelView = glm::translate(this->matrix.modelView, -game.player.segments[0]); 
	glm::vec2 sin = glm::sin(glm::radians(this->rotation));
	glm::vec2 cos = glm::cos(glm::radians(this->rotation));
	glm::vec3 dir = glm::vec3(-sin.x * cos.y, sin.y, cos.x * cos.y);
	this->matrix.modelView = glm::translate(this->matrix.modelView, dir * -5.0f);
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
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	this->borderShaderProgram.bind();
	this->borderShaderProgram.bindBuffer(GL_UNIFORM_BUFFER, this->renderEngine.globalUBO, "Global");
	this->borderVAO.bind();
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

RenderEngine::RenderEngine(): 
	skyboxRenderer(*this), 
	buffer(1024 * 1024 * 8),
	genericDrawShaderProgram("resources/shaders/GenericDraw.vert.glsl", "resources/shaders/GenericDraw.frag.glsl"), 
	worldObjVertexCount(0), snakeVertexCount(0) {
	globalUBO.allocate(268, GL_DYNAMIC_STORAGE_BIT);
}

void setupUBO(RenderEngine& renderEngine, GameWindow& gameWindow, float tickDelta) {
	char mem[268];
	size_t i = 0;
	memcpy(mem + i, &renderEngine.camera.matrix, sizeof(ProjViewModelMatrix));
	i += sizeof(ProjViewModelMatrix);

	glm::mat4 inverseProjectionMatrix = glm::inverse(renderEngine.camera.matrix.projection);
	glm::mat4 inverseModelViewMatrix = glm::inverse(renderEngine.camera.matrix.modelView);
	memcpy(mem + i, &inverseProjectionMatrix, sizeof(glm::mat4));
	i += sizeof(glm::mat4);
	memcpy(mem + i, &inverseModelViewMatrix, sizeof(glm::mat4));
	i += sizeof(glm::mat4);
	glm::f32vec2 windowSize((float) gameWindow.windowSize.x, (float) gameWindow.windowSize.y);
	memcpy(mem + i, &windowSize, sizeof(glm::vec2));
	i += sizeof(glm::vec2);

	mem[i] = tickDelta;

	renderEngine.globalUBO.invalidate();
	glNamedBufferSubData(renderEngine.globalUBO.id, 0, 268, mem);
}

void setupMesh(RenderEngine& renderEngine, Game& game, float tickDelta) {
	renderEngine.buffer.update();
	std::vector<ItemObj>& worldObjs = game.world.objects;
	for (auto& obj : worldObjs) {
		switch (obj.item) {
			case Item::Food:
				fillFoodMeshInterleaved(renderEngine.buffer, obj.pos, obj.radius, { 1.0f, 0.0f, 0.0f, 1.0f });
				break;
			default:
				// ADD more
				break;
		}
	}
	renderEngine.worldObjVAO.clearAttachments();
	renderEngine.worldObjVAO.attachVertexBuffer(
		renderEngine.buffer.buffer,
		renderEngine.buffer.offset(),
		OpenGL::VertexAttribute::Builder(40)
		.addFloat(0, 3, GL_FLOAT, false)
		.addFloat(1, 3, GL_FLOAT, false)
		.addFloat(2, 4, GL_FLOAT, false)
		.build()
	);
	renderEngine.worldObjVertexCount = renderEngine.buffer.size / 40;
	renderEngine.buffer.finish();

	fillSnakeMeshInterleaved(game.player.segments, renderEngine.buffer, Snake::radius, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	renderEngine.snakeVAO.clearAttachments();
	renderEngine.snakeVAO.attachVertexBuffer(
		renderEngine.buffer.buffer,
		renderEngine.buffer.offset(),
		OpenGL::VertexAttribute::Builder(40)
		.addFloat(0, 3, GL_FLOAT, false)
		.addFloat(1, 3, GL_FLOAT, false)
		.addFloat(2, 4, GL_FLOAT, false)
		.build()
	);
	renderEngine.snakeVertexCount = renderEngine.buffer.size / 40;
	renderEngine.buffer.finish();
}

void RenderEngine::setup(GameWindow& gameWindow, Game& game, float tickDelta) {
	setupUBO(*this, gameWindow, tickDelta);
	setupMesh(*this, game, tickDelta);
}

void RenderEngine::render(GameWindow& gameWindow, float tickDelta) {
	glEnable(GL_DEPTH_TEST);
	this->genericDrawShaderProgram.bind();
	this->genericDrawShaderProgram.bindBuffer(GL_UNIFORM_BUFFER, this->globalUBO, "Global");
	this->worldObjVAO.bind();
	glDrawArrays(GL_TRIANGLES, 0, this->worldObjVertexCount);
	this->snakeVAO.bind();
	glDrawArrays(GL_TRIANGLES, 0, this->snakeVertexCount);
}

PersistentMappedBuffer::PersistentMappedBuffer(GLsizeiptr size) : frame(0), size(0) {
	this->buffer.allocate(size, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
	this->originPtr = (char8_t*) glMapNamedBufferRange(
		this->buffer.id,
		0,
		size,
		GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT | GL_MAP_UNSYNCHRONIZED_BIT
	);
	this->pointer = this->originPtr;
}

void PersistentMappedBuffer::update() {
	frame++;
	if (frame == 2) {
		pointer = originPtr;
		frame = 0;
	}
}

void PersistentMappedBuffer::finish() {
	pointer += size;
	size = 0;
}

GLuint PersistentMappedBuffer::offset() const {
	return (GLuint) (pointer - originPtr);
}
