#ifndef GUI_H
#define GUI_H

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "glm/glm.hpp"
#include "renman.h"
#include <string>
#include <vector>
#include "collisionman.h"

extern bool starteditor;
extern bool lodproj;
extern bool changesettings;
extern bool launchie;
extern int winwidth;
extern int winheight;
extern int graphicsquality;

extern float gravity;

extern float farplane;

extern BVHNode* BVHtree;

extern std::string outprojectname;
extern std::vector<std::string> filenames;

class GUI
{
    public:
        void earlyInit(GLFWwindow* window);
        void earlyStartFrame();
        void earlyUpdate();
        void earlyrout();
        void earlyShutdown();
        void Init(GLFWwindow* window, const char* glsl_version);
        void Shutdown();
        void StartFrame();
        void rout();
        void Update(GLFWwindow* window, Camera& cam);
    protected:
    private:
        Object Hmodel;

};

#endif
