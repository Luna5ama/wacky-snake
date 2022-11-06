#pragma once

#include "game/Game.hpp"
#include <glm/glm.hpp>
#include "GLObjects.hpp"
#include "Main.hpp"

class RenderEngine;

struct ProjViewModelMatrix {
	glm::mat4 projection;
	glm::mat4 modelView;
};

class Camera {
public:
	ProjViewModelMatrix matrix;
	glm::vec2 rotation;
	float fov;

	Camera();

	void updateProjection(GameWindow& gameWindow);
	void updateModelView(Game& game, glm::vec2 mousePosDelta);
};

struct SkyboxRenderer {
	RenderEngine& renderEngine;

	OpenGL::ShaderProgram borderShaderProgram;
	OpenGL::VertexArrayObject borderVAO;
	OpenGL::BufferObject::Immutable borderVBO;

	SkyboxRenderer(RenderEngine& renderEngine);

    void render(GameWindow& gameWindow);
};

class PersistentMappedBuffer {
private:
	uint32_t frame;
public:
	OpenGL::BufferObject::Immutable buffer;
	char8_t* originPtr;
	char8_t* pointer;
	size_t size;

	PersistentMappedBuffer(GLsizeiptr size);
	void update();
	void finish();
	GLuint offset() const;
};

class RenderEngine {
public:
	Camera camera;
	OpenGL::BufferObject::Immutable globalUBO;
	PersistentMappedBuffer buffer;
	SkyboxRenderer skyboxRenderer;
	
	OpenGL::ShaderProgram genericDrawShaderProgram;
	OpenGL::VertexArrayObject worldObjVAO;
	GLsizei worldObjVertexCount;
	OpenGL::VertexArrayObject snakeVAO;
	GLsizei snakeVertexCount;
	
	RenderEngine();
	
	void setup(GameWindow& gameWindow, Game& game, float tickDelta);
	void render(GameWindow& gameWindow, float tickDelta);
};
