#pragma once

#include <glm/glm.hpp>
#include "GLObjects.hpp"
#include "Main.h"

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
	void updateView(glm::vec2 mousePosDelta);
};

struct SkyboxShaderProgram : public OpenGL::ShaderProgram {
	GLuint mouseUniform;

	SkyboxShaderProgram();
};

struct SkyboxRenderer {
	RenderEngine& renderEngine;
	
	SkyboxShaderProgram skyboxShaderProgram;
	OpenGL::VertexArrayObject skyboxVAO;
	OpenGL::BufferObject::Immutable skyboxVBO;

	OpenGL::ShaderProgram borderShaderProgram;
	OpenGL::VertexArrayObject borderVAO;
	OpenGL::BufferObject::Immutable borderVBO;

	SkyboxRenderer(RenderEngine& renderEngine);

    void render(GameWindow& gameWindow);
};

class RenderEngine {
public:
	Camera camera;
	OpenGL::BufferObject::Immutable globalUBO;
	SkyboxRenderer skyboxRenderer;
	
	RenderEngine();
	
	void setup(GameWindow& gameWindow, float tickDelta);
};
