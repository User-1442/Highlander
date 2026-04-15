#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "glm/glm.hpp"
#include "renman.h"
#include "GUI.h"
#include "renman.h"
#include "objloader.h"
#include "Scene.h"
#include <string>
#include <iostream>
#include <vector>
#include "Saveman.h"
#include <cstring>
#include <filesystem>

namespace fs = std::filesystem;

bool starteditor = false;
bool lodproj = false;
bool deletepressed = false;
bool wascopied = false;
bool iscopied = false;
bool waspasted = false;
bool ispasted = false;
bool vsync = false;
bool launchgsm = false;
bool changesettings = false;

int winwidth;
int winheight;

int page = 0;
int gsmpage = 0;

int graphicsquality = 2;

char name[256];
char diffusepath[256];
char normalpath[256];
char armpath[256];
char savepath[256];
char savename[256];
char objname[256];

unsigned int guiloopnumber = 0;

std::string projectname;
std::string deletefile;

std::vector<std::string> filenames;
std::vector<std::string> copiedfilenames;
std::vector<std::string> available_projects;
std::vector<Object> copiedobjects;
std::vector<int> loopedobj;
std::vector<int> loopingobj;

const char* items[] = {"Add OBJ", "More coming soon"};
glm::vec3 LightColorlocal = glm::vec3(0.0f);
int current_item_index = 0;
glm::vec3 objectcolor = glm::vec3(1.0f, 1.0f, 1.0f);

void GUI::earlyInit(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiWindowFlags_NoResize;
    ImGuiWindowFlags_NoCollapse;
    ImGuiWindowFlags_NoMove;
    ImGuiWindowFlags_NoTitleBar;
    ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 420");

    ImGuiIO& io = ImGui::GetIO(); (void)io;

    io.IniFilename = "splash.ini";

    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    std::string currentdir = "./";

    for (const auto & entry : fs::directory_iterator(currentdir)) {
        if (fs::is_directory(entry.path())) {
            std::cout << entry.path() << std::endl;
            std::string entrypath = (entry.path()).string();
            available_projects.push_back(entrypath);
        }
    }
}

void GUI::earlyStartFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void GUI::earlyUpdate() {
    starteditor = false;
    ImGui::DockSpaceOverViewport();
    if (ImGui::Begin("ISM 1.1")) {
        if (page == 0) {
            ImGui::Text("Projects");
            for (int i = 0; i < available_projects.size(); i++) {
                projectname = available_projects[i];
                projectname.erase(projectname.begin(), projectname.begin() + 2);
                if (ImGui::Button(projectname.c_str())) {
                    filenames.clear();
                    Objects.clear();
                    lodproj = true;
                    starteditor = true;
                }
                ImGui::SameLine();
                std::string currentbuttonid = "Delete##" + std::to_string(i);
                if (ImGui::Button(currentbuttonid.c_str())) {
                    deletepressed = true;
                    deletefile = available_projects[i];
                    deletefile.erase(deletefile.begin(), deletefile.begin() + 2);
                }
            }
            if (deletepressed == true) {
                if (ImGui::Begin("Deleteman")) {
                    ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2);
                    std::string deletemessage = "Are you sure you want to delete " + deletefile + "?";
                    ImGui::Text(deletemessage.c_str());
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 0.2f, 0.2f, 1.0f));
                    if (ImGui::Button("Yes")) {
                        fs::remove_all(deletefile);
                        deletepressed = false;
                    }
                    ImGui::PopStyleColor();
                    ImGui::SameLine();
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 0.2f, 1.0f));
                    if (ImGui::Button("No")) {
                        deletepressed = false;
                    }
                    ImGui::PopStyleColor();
                    ImGui::End();
                }
            }
            ImGui::Separator();
            if (ImGui::Button("New Project")) {
                starteditor = true;
            }
            ImGui::Separator();
            if (ImGui::Button("Settings")) {
                page = 1;
            }
        }
        if (page == 1) {
            if (ImGui::BeginCombo("Resolution", "Resolution")) {
                if (ImGui::Button("3840 x 2160")) {
                    winwidth = 3840;
                    winheight = 2160;
                }
                if (ImGui::Button("2560 x 1440")) {
                    winwidth = 2560;
                    winheight = 1440;
                }
                if (ImGui::Button("1920 x 1080")) {
                    winwidth = 1920;
                    winheight = 1080;
                }
                if (ImGui::Button("1280 x 720")) {
                    winwidth = 1280;
                    winheight = 720;
                }
                if (ImGui::Button("720 x 480")) {
                    winwidth = 720;
                    winheight = 480;
                }
                ImGui::EndCombo();
            }
            if (ImGui::BeginCombo("Graphics", "Graphics Quality")) {
                if (ImGui::Button("Ultra")) {
                    graphicsquality = 0;
                    changesettings = true;
                }
                if (ImGui::Button("High")) {
                    graphicsquality = 1;
                    changesettings = true;
                }
                if (ImGui::Button("Medium")) {
                    graphicsquality = 2;
                    changesettings = true;
                }
                if (ImGui::Button("Low")) {
                    graphicsquality = 3;
                    changesettings = true;
                }
                if (ImGui::Button("Potato")) {
                    graphicsquality = 4;
                    changesettings = true;
                }
                ImGui::EndCombo();
            }
            if (ImGui::Checkbox("Vsync", &vsync)) {
                glfwSwapInterval(1);
            } else {
                glfwSwapInterval(0);
            }
            if (ImGui::Button("Home")) {
                page = 0;
            }
        }
        ImGui::End();
    }
}

void GUI::earlyrout() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
}

void GUI::earlyShutdown() { //Used to be Kill yourself
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void GUI::Init(GLFWwindow* window, const char* glsl_version) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 420");

    if (vsync) {
        glfwSwapInterval(1);
    } else {
        glfwSwapInterval(0);
    }

    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
}

void GUI::Shutdown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void GUI::StartFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void GUI::rout() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
}

void GUI::Update(GLFWwindow* window) {
    //provide the whole background for ImGUI to use
    ImGui::DockSpaceOverViewport();

    bool objectselected = false;

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::BeginMenu("Save")) {
                ImGui::InputText("Project Name", savename, 256);
                if (ImGui::Button("Save")) {
                    std::cout << "DEBUG: About to call SaveProject with " << filenames.size() << " files." << std::endl;
                    SaveProject(savename, filenames);
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Import")) {
                if (ImGui::BeginMenu("Project")) {
                    for (int i = 0; i < available_projects.size(); i++) {
                        projectname = available_projects[i];
                        projectname.erase(projectname.begin(), projectname.begin() + 2);
                        if (ImGui::Button(projectname.c_str())) {
                            filenames.clear();
                            Objects.clear();
                            LoadProject(projectname, filenames);
                        }
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Settings")) {
            if (ImGui::Button("Global Setting Manager (GSM)")) {
                launchgsm = true;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
    if (launchgsm == true) {
        if (ImGui::Begin("GSM 1.0")) {
            if (gsmpage == 0) {
                ImGui::Text("Global Settings Manager");
                if (ImGui::Button("Lighting")) {
                    gsmpage = 1;
                }
                if (ImGui::Button("Performance")) {
                    gsmpage = 2;
                }
                if (ImGui::Button("Close")) {
                    launchgsm = false;
                }
            }
            if (gsmpage == 1) {
                ImGui::Text("Global Lighting");
                ImGui::SliderFloat("Reflectiveness", &reflectiveness, 0.5f, 100.0f);
                ImGui::ColorEdit3("Ambient", &ambientlight[0]);
                ImGui::ColorEdit3("Specular", &spec[0]);
                if (ImGui::Button("Apply Settings")) {
                    InitUniforms();
                }
                if (ImGui::Button("Reset to Defaults")) {
                    reflectiveness = 30;
                    ambientlight = glm::vec3(0.2, 0.2, 0.2);
                    spec = glm::vec3(1, 1, 1);
                    LightColorlocal = glm::vec3(1, 1, 1);
                    InitUniforms();
                }
                if (ImGui::Button("Back")) {
                    gsmpage = 0;
                }
            }
            if (gsmpage == 2) {
                if (ImGui::BeginCombo("Graphics", "Graphics Quality")) {
                    if (ImGui::Button("Ultra")) {
                        graphicsquality = 0;
                    }
                    if (ImGui::Button("High")) {
                        graphicsquality = 1;
                    }
                    if (ImGui::Button("Medium")) {
                        graphicsquality = 2;
                    }
                    if (ImGui::Button("Low")) {
                        graphicsquality = 3;
                    }
                    if (ImGui::Button("Potato")) {
                        graphicsquality = 4;
                    }
                    ImGui::EndCombo();
                }
                if (ImGui::Button("Apply Graphics Settings")) {
                    Applysettings();
                }
                if (ImGui::Button("Back")) {
                    gsmpage = 0;
                }
            }
            ImGui::End();
        }
    }

    if (ImGui::Begin("Highlander Viewport"))
    {
        ImVec2 viewportSize = ImGui::GetContentRegionAvail();
        //ImGui::Image((void*)(intptr_t)renderedTexture, viewportSize, ImVec2(0,1), ImVec2(1, 0));
        // Change renderedTexture to depthTexture
        ImGui::Image((void*)(intptr_t)actualFinalTexture, viewportSize, ImVec2(0,1), ImVec2(1, 0));
        ImGui::End();
    }
    if (ImGui::Begin("Editor")) {
        if (ImGui::BeginTabBar("Editor")) {
            /*if (ImGui::BeginTabItem("Materials"))
            {
                if (ImGui::BeginCombo("File", "File"))
                {
                    if (ImGui::BeginCombo("Save Project", "Save Project"))
                    {
                        ImGui::InputText("Project Name", savename, 256);
                        if (ImGui::Button("Save")) {
                            std::cout << "DEBUG: About to call SaveProject with " << filenames.size() << " files." << std::endl;
                            SaveProject(savename, filenames);
                        }
                        ImGui::EndCombo();
                    }
                    if (ImGui::BeginCombo("Open Project", "Open Project")) {
                        ImGui::InputText("Load .Highlander project", savepath, 256);
                        if (ImGui::Button("Load")) {
                            std::cout << "Attempting to initialize Loading process";
                            if (savepath[0] == '\0') {
                                std::cout << "UI Error: savepath is empty!" << std::endl;
                            } else {
                                LoadProject(savepath, filenames);
                                std::cout << "UI: LoadProject finished!" << std::endl;
                            }
                        }
                        ImGui::EndCombo();
                    }
                    ImGui::EndCombo();
                }
                ImGui::EndTabItem();
            }*/
            if (ImGui::BeginTabItem("Objects")) {
                ImGui::Text("New");
                ImGui::InputText("Add New Obj", objfile, 256);
                const char* combo_id = "AddCombo";
                static const char* preview = "Add";
                if (ImGui::BeginCombo("Add New...", "Select Item type"))
                {
                    if (ImGui::BeginCombo("Object", "Object")) {
                        if (ImGui::Button("Add OBJ")) {
                            positions.clear();
                            indices.clear();
                            uvs.clear();
                            normals.clear();
                            tangents.clear();

                            Hmodel.name = "Object" + std::to_string(Objects.size());

                            std::cout << "loading object vertices with objload" << std::endl;
                            Hmodel.vertexCount = objload(objfile, positions, indices, uvs, normals, tangents);
                            Hmodel.vertices = positions;
                            Hmodel.indices = indices;
                            Hmodel.objtangents = tangents;
                            Hmodel.uvs = uvs;
                            Hmodel.normals = normals;
                            Hmodel.color = objectcolor;

                            filenames.push_back(std::string(objfile));

                            std::cout << "Loading Object with OBJ" << std::endl;
                            CreateObjectWithOBJ(Hmodel);

                            Objects.push_back(Hmodel);

                            UpdateBuffers(Objects.back());
                            std::cout << "Successfull! :D" << std::endl;
                        }
                        ImGui::EndCombo();
                    }
                    if (ImGui::BeginCombo("Light", "Light"))
                    {
                        if (ImGui::Button("Add Point Light")) {
                            Light newlight;
                            newlight.name = "Light" + std::to_string(SceneLights.size());
                            SceneLights.push_back(newlight);
                        }
                        ImGui::EndCombo();
                    }
                    ImGui::EndCombo();
                }
                ImGui::Separator();
                ImGui::Text("Object Properties");
                for (int i = Objects.size() - 1; i > -1; i--) {
                    Object& obj = Objects[i];
                    if (obj.selected) {
                        objectselected = true;
                        break;
                    }
                }
                if (objectselected == true) {
                    std::string objectname;

                    glm::vec3 color;
                    glm::vec3 ogcolor;
                    glm::vec3 rotation = glm::vec3(0.0f);
                    glm::vec3 originalrotation = glm::vec3(0.0f);
                    glm::vec3 scale = glm::vec3(1.0f);
                    glm::vec3 originalscale = glm::vec3(1.0f);

                    unsigned int diffuseMap;
                    unsigned int normalMap;
                    unsigned int armMap;

                    char diffusepath[256] = "\0";
                    char originaldiffusepath[256] = "\0";
                    char normalpath[256] = "\0";
                    char originalnormalpath[256] = "\0";
                    char armpath[256] = "\0";
                    char originalarmpath[256] = "\0";

                    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
                    glm::vec3 originalpos = glm::vec3(0.0f);

                    bool armchanged = false;
                    bool normalchanged = false;
                    bool diffusechanged = false;
                    bool namechanged = false;

                    bool deleteobjects = false;

                    loopingobj.clear();

                    int activeobject = -1;

                    for (int i = Objects.size() - 1; i > -1; i--) {
                        Object& obj = Objects[i];
                        if (obj.selected) {
                            activeobject = i;
                        }
                    }
                    Object& activeobj = Objects[activeobject];
                    originalpos = activeobj.position;
                    originalrotation = activeobj.rotation;
                    originalscale = activeobj.scale;
                    loopedobj = loopingobj;
                    ImGui::Text("Name");
                    memcpy(objname, activeobj.name.c_str(), 256);
                    ImGui::InputText("Name", objname, sizeof(objname));
                    if (ImGui::Button("Change Name")) {
                        namechanged = true;
                        objectname = activeobj.name;
                    }
                    ImGui::Text("Transform Controls");
                    ImGui::DragFloat3("Position", &activeobj.position.x, 0.1f);
                    ImGui::DragFloat3("Rotation", &activeobj.rotation.x, 0.1f);
                    ImGui::DragFloat3("Scale", &activeobj.scale.x, 0.1f);
                    ImGui::Text("Color/Texture");
                    ImGui::ColorEdit3("Base Color", &color.x);
                    ImGui::InputText("Albedo", activeobj.diffusepath, sizeof(activeobj.diffusepath));
                    if (ImGui::Button("Load Albedo")) {
                        activeobj.diffuseMap = LoadTextures(activeobj.diffusepath, true);
                        diffusechanged = true;
                    }
                    ImGui::InputText("Normal", activeobj.normalpath, sizeof(activeobj.normalpath));
                    if (ImGui::Button("Load Normals")) {
                        activeobj.normalMap = LoadTextures(activeobj.normalpath, false); //
                        normalchanged = true;
                    }
                    ImGui::InputText("ARM Map (combine AO Roughness and Metallic)", activeobj.armpath, sizeof(activeobj.armpath));
                    if (ImGui::Button("Load ARM")) {
                        activeobj.armMap = LoadTextures(activeobj.armpath, false);
                        armchanged = true;
                    }
                    if (ImGui::Button("Delete Object")) {
                        deleteobjects = true;
                    }
                    position = activeobj.position - originalpos;
                    //scale -= activeobj.scale - originalscale;
                    rotation -= activeobj.rotation - originalrotation;
                    for (int i = Objects.size() - 1; i > -1; i--) {
                        Object& obj = Objects[i];
                        if (obj.selected && i != activeobject) {
                            if (deleteobjects == true) {
                                Objects.erase(Objects.begin() + i);
                                filenames.erase(filenames.begin() + i);
                            } else {
                                Object& obj = Objects[i];
                                if (namechanged == true) {
                                    obj.name = activeobj.name;
                                }
                                if (ogcolor != color) {
                                    obj.color = activeobj.color;
                                }
                                obj.position += position;
                                obj.scale = activeobj.scale;
                                obj.rotation += rotation;
                                if (diffusechanged == true) {
                                    obj.diffuseMap = activeobj.diffuseMap;
                                    memcpy(obj.diffusepath, activeobj.diffusepath, 256);
                                }
                                if (normalchanged == true) {
                                    obj.normalMap = activeobj.normalMap;
                                    memcpy(obj.normalpath, activeobj.normalpath, 256);
                                }
                                if (armchanged == true) {
                                    obj.armMap = activeobj.armMap;
                                    memcpy(obj.armpath, activeobj.armpath, 256);
                                }
                            }
                        }
                    }
                    //don't know how to combine everything into ARM so...
                }
                    ImGui::Separator();
                    ImGui::Text("Light list");
                    for (int i = 0; i < SceneLights.size(); i++) {
                        if (ImGui::TreeNode((void*)(intptr_t)i, SceneLights[i].name.c_str())) {
                            ImGui::DragFloat3("Position", &SceneLights[i].position.x, 0.1f);
                            ImGui::ColorEdit3("Color", &SceneLights[i].color.r);
                            ImGui::SliderFloat("Brightness", &SceneLights[i].brightness, 0.0f, 100.0f);

                            if (ImGui::Button("Delete Light")) {
                            SceneLights.erase(SceneLights.begin() + i);
                            }
                            ImGui::TreePop();
                        }
                    }
                    ImGui::Separator();
                    ImGui::Text("Object List");
                    for (int i = 0; i < Objects.size(); i++) {
                        std::string label = Objects[i].name + "##" + std::to_string(i);

                        if (ImGui::Selectable(label.c_str(), Objects[i].selected)) {
                            if (!ImGui::GetIO().KeyShift) {
                                for(auto& o : Objects) o.selected = false;

                                Objects[i].selected = true;
                            } else {
                                Objects[i].selected = !Objects[i].selected;
                            }
                        }
                    }
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Stats")) {
                const char* FPSstring = std::to_string(FPS).c_str();
                ImGui::Text(FPSstring);
                ImGui::EndTabItem();
            }
                };

            }
        ImGui::EndTabBar();
        ImGui::End();
    if (ImGui::Begin("IndevLayout2")) {
        ImGui::Text("Stuff Will be here soon! Might be a file manager or smth idk yet");
        ImGui::End();
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        iscopied = true;
        if (iscopied && !wascopied) {
            copiedobjects.clear();
            copiedfilenames.clear();
            for (int i = 0; i < Objects.size(); i++) {
                Object& obj = Objects[i];
                if (obj.selected) {
                    copiedobjects.push_back(obj);
                    copiedfilenames.push_back(filenames[i]);
                    iscopied = true;
                }
            }
        }
    } else {
        iscopied = false;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
        ispasted = true;
        if (copiedobjects.size() != 0) {
            if (ispasted && !waspasted) {
                for (int i = 0; i < copiedobjects.size(); i++) {
                    Objects.push_back(copiedobjects[i]);
                    filenames.push_back(copiedfilenames[i]);
                    ispasted = true;
                }
            }
        }
    } else {
        ispasted = false;
    }
    waspasted = ispasted;
    wascopied = iscopied;
    guiloopnumber++;
}
