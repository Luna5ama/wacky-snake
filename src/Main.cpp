#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "game/Game.hpp"

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
Game game{};

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        switch (key)
        {
        case GLFW_KEY_W:
            game.player.setDirection(glm::vec3(0.0, 0.0, 1.0));
            break;
        case GLFW_KEY_A:
            game.player.setDirection(glm::vec3(-1.0, 0.0, 0.0));
            break;
        case GLFW_KEY_S:
            game.player.setDirection(glm::vec3(0.0, 0.0, -1.0));
            break;
        case GLFW_KEY_D:
            game.player.setDirection(glm::vec3(1.0, 0.0, 0.0));
            break;

        case GLFW_KEY_SPACE:
            game.player.setDirection(glm::vec3(0.0, 1.0, 0.0));
            break;
        case GLFW_KEY_LEFT_SHIFT:
            game.player.setDirection(glm::vec3(0.0, -1.0, 0.0));
            break;
        default:
            break;
        }
    }
}

int main() {
    std::string title = "3D Snake";

    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

    glm::i32vec2 prevWindowSize{ -1, -1 };
    glm::i32vec2 windowSize{ 1280, 720 };

    GLFWwindow* window = glfwCreateWindow(windowSize.x, windowSize.y, title.c_str(), NULL, NULL);
    glfwMakeContextCurrent(window);

    glewInit();

    // Double buffered V-Sync
    glfwSwapInterval(1);
    glfwWindowHint(GLFW_DOUBLEBUFFER, true);

    // Mouse/Keyboard callbacks
    //glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetKeyCallback(window, keyCallback);

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(messageCallback, 0);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glEnable(GL_CULL_FACE);

    // Setup here

    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "Pre render OpenGL error: " << err << std::endl;
    }

    double curTime = glfwGetTime();

    // game initialization
    game.placeFood(10);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        // Process inputs here

        double time = glfwGetTime();
        double dt = time - curTime;
        curTime = time;
        game.tick(dt);


        glfwGetWindowSize(window, &windowSize.x, &windowSize.y);

        if (prevWindowSize != windowSize && windowSize.x != 0 && windowSize.y != 0) {
            prevWindowSize = windowSize;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, windowSize.x, windowSize.y);

        // Render goes here

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR) {
            std::cerr << "OpenGL error: " << err << std::endl;
        }

        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
}