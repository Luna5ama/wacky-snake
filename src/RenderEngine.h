#pragma once

#include "GLObjects.hpp"

struct ProjViewModelMatrix {
	glm::mat4 projection;
	glm::mat4 modelView;
};

class Camera {
public:
	ProjViewModelMatrix matrix;
	glm::vec2 rotation;
	float fov;

	void updateProjection(glm::vec2 windowSize);
	void updateView(glm::vec2 mousePosDelta);
};

struct SkyboxShaderProgram : public OpenGL::ShaderProgram {
	GLuint resolutionUniform;
	GLuint mouseUniform;

	SkyboxShaderProgram();
};

struct SkyboxRenderer {
	SkyboxShaderProgram skyboxShaderProgram;
	OpenGL::VertexArrayObject skyboxVAO;
	OpenGL::BufferObject::Immutable skyboxVBO;

	OpenGL::ShaderProgram borderShaderProgram;
	OpenGL::VertexArrayObject borderVAO;
	OpenGL::BufferObject::Immutable borderVBO;

	SkyboxRenderer();

    void render() const;
};

class RenderEngine {
private:
	SkyboxShaderProgram skyboxShaderProgram;
public:
	Camera camera;
	
	void renderSkybox();
};
