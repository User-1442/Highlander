#ifndef WINMAN_H
#define WINMAN_H

#include "glfw3.h"


class winman
{
    public:
        winman(int width, int height, const char* title);
        void Update();
        bool ShouldClose();
        GLFWwindow* GetWindow() const { return window; };
    private:
        GLFWwindow* window;
};

#endif // WINMAN_H
