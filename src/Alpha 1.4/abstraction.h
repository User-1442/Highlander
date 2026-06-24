#pragma once
#include "glfw3.h"
#include "Scene.h"
#include "Camera.h"

extern bool usevulkan;

void InitRenderer(GLFWwindow* window, int width, int height);
void RenderLoop(GLFWwindow* window, int width, int height, Camera& cam);
void InitializeUniforms();
