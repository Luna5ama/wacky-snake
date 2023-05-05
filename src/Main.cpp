

#include <iostream>
#include <string>

#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include "RenderEngine.hpp"
#include "game/Game.hpp"

#include "Main.hpp"

// Debug message handler
void GLAPIENTRY messageCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam) {
	fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		type, severity, message);
}

// game instance
constexpr int INITIAL_FOODS = 1000;
Game game{};
RenderEngine* renderEnginePtr;

bool controlled = false;
bool wireframe = false;

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_LEFT_CONTROL) {
		if (action == GLFW_PRESS) {
			controlled = true;
		}
		else if (action == GLFW_RELEASE) {
			controlled = false;
		}
	}
	else if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		glm::vec2 cameraRotation = renderEnginePtr->camera.rotation;
		cameraRotation.x += 180.0f;
		cameraRotation.y = 0.0f;
		cameraRotation = glm::round(cameraRotation / 90.0f) * 90.0f;
		switch (key) {
			case GLFW_KEY_W:
				game.player.setRotation(cameraRotation + glm::vec2(0.0f, 0.0f));
				break;
			case GLFW_KEY_A:
				game.player.setRotation(cameraRotation + glm::vec2(-90.0f, 0.0f));
				break;
			case GLFW_KEY_S:
				game.player.setRotation(cameraRotation + glm::vec2(180.0f, 0.0f));
				break;
			case GLFW_KEY_D:
				game.player.setRotation(cameraRotation + glm::vec2(90.0f, 0.0f));
				break;

			case GLFW_KEY_SPACE:
				game.player.setRotation(glm::vec2(0.0f, 90.0f));
				break;
			case GLFW_KEY_LEFT_SHIFT:
				game.player.setRotation(glm::vec2(0.0f, -90.0f));
				break;

			case GLFW_KEY_ESCAPE:
				glfwSetWindowShouldClose(window, GL_TRUE);
				break;


				// debug controls
			case GLFW_KEY_T: // wireframe toggle
				if (controlled) {
					wireframe = !wireframe;
					if (wireframe) {
						glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
					}
					else {
						glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					}
				}
				break;
			case GLFW_KEY_R: // restart
				if (controlled) {
					game.player = Snake();
					game.state = State::Waiting;
					game.timeElapsed = 0.0;
					game.world.objects = {};
					game.placeFood(INITIAL_FOODS);
				}
				break;
			case GLFW_KEY_F: // force end
				if (controlled) {
					game.state = State::Overing;
				}
				break;
			case GLFW_KEY_C: // force continue
				if (controlled) {
					game.state = State::Playing;
				}
				break;
			default:
				break;
		}
	}
}

int main() {
	std::string title = "Wacky Snake";

	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	GameWindow gameWindow{ {-1, -1}, {1280, 720}, nullptr };
	gameWindow.window = glfwCreateWindow(gameWindow.windowSize.x, gameWindow.windowSize.y, title.c_str(), NULL, NULL);
	glfwMakeContextCurrent(gameWindow.window);

	glewInit();

	// Double buffered V-Sync
	glfwSwapInterval(1);
	glfwSetInputMode(gameWindow.window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
	glfwSetInputMode(gameWindow.window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	glfwPollEvents();

	// Mouse/Keyboard callbacks
	//glfwSetMouseButtonCallback(window, mouseButtonCallback);

	glfwSetKeyCallback(gameWindow.window, keyCallback);
	glm::f64vec2 prevMousePos;
	glfwGetCursorPos(gameWindow.window, &prevMousePos.x, &prevMousePos.y);
	glm::f64vec2 mousePos;

#ifdef _DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(messageCallback, 0);
#endif

	// Setup here
	RenderEngine renderEngine;
	renderEnginePtr = &renderEngine;

	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR) {
		std::cerr << "Pre render OpenGL error: " << err << std::endl;
	}


	double curTime = glfwGetTime();
	double lastTickTime = curTime;

	// game initialization
	game.placeFood(INITIAL_FOODS);

	while (!glfwWindowShouldClose(gameWindow.window)) {
		glfwPollEvents();
		// Process inputs here
		glm::f64vec2 center = (glm::f64vec2) gameWindow.windowSize / 2.0;
		glfwGetCursorPos(gameWindow.window, &mousePos.x, &mousePos.y);
		glm::f32vec2 mouseDelta = mousePos - prevMousePos;
		glfwSetCursorPos(gameWindow.window, center.x, center.y);
		glfwGetCursorPos(gameWindow.window, &prevMousePos.x, &prevMousePos.y);

		double time = glfwGetTime();
		double dt = time - curTime;
		curTime = time;
		game.tick(dt);

		glfwGetWindowSize(gameWindow.window, &gameWindow.windowSize.x, &gameWindow.windowSize.y);

		if (gameWindow.prevWindowSize != gameWindow.windowSize
			&& gameWindow.windowSize.x != 0 && gameWindow.windowSize.y != 0) {
			gameWindow.prevWindowSize = gameWindow.windowSize;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, gameWindow.windowSize.x, gameWindow.windowSize.y);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		// Render goes here
		renderEngine.camera.updateProjection(gameWindow);
		renderEngine.camera.updateModelView(game, mouseDelta);
		renderEngine.setup(gameWindow, game, dt);
		renderEngine.render(gameWindow, dt);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		GLenum err;
		while ((err = glGetError()) != GL_NO_ERROR) {
			std::cerr << "OpenGL error: " << err << std::endl;
		}

		glfwSwapBuffers(gameWindow.window);
	}

	glfwDestroyWindow(gameWindow.window);
}