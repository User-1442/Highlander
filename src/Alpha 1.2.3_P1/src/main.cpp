#include "glad.h"
#include <iostream>
#include "winman.h"
#include "renman.h"
#include "Camera.h"
#include "GUI.h"
#include "objloader.h"
#include <string>
#include <vector>
#include "Saveman.h"

GLuint width = 1920;
GLuint height = 1080;

std::vector<Object> Objects; //make sure the linker doesn't cry

int main() {
    winwidth = width;
    winheight = height;
    //Window CLASS
    winman initwindow(width, height, "Alpha 1.2.3");
    GUI initgui;
    initwindow.Update();
    if (!gladLoadGL()) {
        std::cout << "GLAD::FAILED::SRY::BRO:: ERROR::3";
    }
    glfwSwapInterval(0);
    initgui.earlyInit(initwindow.GetWindow());
    while (!initwindow.ShouldClose()) {
        glfwPollEvents();
        initgui.earlyStartFrame();
        initwindow.Update();
        initgui.earlyUpdate();
        initgui.earlyrout();
        glfwSwapBuffers(initwindow.GetWindow());
        if (starteditor == true) {
            initgui.earlyShutdown();
            glfwDestroyWindow(initwindow.GetWindow());
            break;
        }
    }
    width = winwidth;
    height = winheight;
    winman Window(width, height, "Alpha 1.2.3");
    GUI gui;
    Window.Update();
    InitOpenGL(Window.GetWindow(), width, height);
    Camera cam(Window.GetWindow());
    InitUniforms();
    gui.Init(Window.GetWindow(), "#version 420");
    if (lodproj == true && starteditor == true) {
        LoadProject(projectname.c_str(), filenames);
    }
    if (changesettings == true) {
        Applysettings();
    }

    while (!Window.ShouldClose()) {
        glfwPollEvents();
        gui.StartFrame();
        Window.Update();
        cam.Update(Window.GetWindow());
        OpenGLLoop(Window.GetWindow(), width, height, cam);
        gui.Update(Window.GetWindow());
        gui.rout();
        glfwSwapBuffers(Window.GetWindow());
        if (GLFW_KEY_ESCAPE == GLFW_PRESS) {
            break;
        }
    }
    std::cout << "Shutting Down";
    gui.Shutdown();
    glfwTerminate();
    return 0;
}
