#include "abstraction.h"
#include "renman.h"
#include "glfw3.h"
#include "Camera.h"

bool usevulkan = false;

void InitRenderer(GLFWwindow* window, int width, int height) {
    if (usevulkan == false) {
        InitOpenGL(window, width, height);
    } else {
        InitVulkan(window, width, height);
    }
    return;
}

void RenderLoop(GLFWwindow* window, int width, int height, Camera& cam) {
    if (usevulkan == false) {
        OpenGLLoop(window, width, height, cam);
    } else {
        //VulkanLoop(window, width, height, cam);
        glfwTerminate();
    }
}

void InitializeUniforms() {
    if (usevulkan == false) {
        InitUniforms();
    } else {
        return;
    }
}




