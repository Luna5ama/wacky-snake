#pragma once

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

struct GameWindow {
    glm::i32vec2 prevWindowSize;
    glm::i32vec2 windowSize;
    GLFWwindow* window;
};