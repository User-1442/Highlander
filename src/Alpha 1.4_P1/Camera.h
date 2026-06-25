#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include "glfw3.h"
#include "glm/glm.hpp"

class Camera
{
    public:
        Camera(GLFWwindow* win);
        virtual ~Camera();
        void Update(GLFWwindow* window);
        void processMouseMovement(double xoffset, double yoffset);
        void Updateviewmatrix();
        void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
        float initialFoV = 45.0f;
        float FoV = initialFoV - 5;
        double xpos, ypos;
        glm::mat4 ViewMatrix;
        glm::vec3 position;
        glm::vec3 direction;
    protected:
    private:
        // horizontal angle : toward -Z
        float horizontalAngle = 3.14f;
        // vertical angle : 0, look at the horizon
        float verticalAngle = 0.0f;
        // Initial Field of View

        float mouseSpeed = 0.005f;
        GLFWwindow* window;
};

#endif // WINMAN_H
