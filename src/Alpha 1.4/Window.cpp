#include "winman.h"
#include "glfw3.h"
#include <iostream>
#include "GUI.h"

winman::winman(int width, int height, const char* title) {
    if (!glfwInit()) {
        std::cout << "ERROR 1: GLFW::INIT::ERROR";
    }
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_DEPTH_BITS, 32);
    window = glfwCreateWindow(1920, 1080, title, nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "ERROR 2: GLFW::CREATEWINDOW::ERROR";
    }
}

void winman::Update() {
    glfwMakeContextCurrent(window);
    if (ShouldClose()) {
        glfwTerminate();
    }
}

bool winman::ShouldClose() {
    return glfwWindowShouldClose(window);
}
