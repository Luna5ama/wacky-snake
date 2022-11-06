#pragma once

#include "GLObjects.hpp"

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

	void updateProjection(glm::vec2 windowSize);
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

    void render();
};

class RenderEngine {
public:
	OpenGL::BufferObject::Immutable globalUBO;
	Camera camera;
	SkyboxRenderer skyboxRenderer;
	
	RenderEngine();
	
	void setupGlobalUBO(glm::vec2 windowSize, float tickDelta);
};
