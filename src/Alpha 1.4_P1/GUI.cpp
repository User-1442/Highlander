#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "glm/glm.hpp"
#include "renman.h"
#include "abstraction.h"
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
#include "collisionman.h"
#include "Camera.h"
#include "imfilebrowser.h"

BVHNode* BVHtree = nullptr;

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
bool launchie = false;
bool loadOBJ = false;
bool loadFBX = false;
bool loadNormals = false;
bool loadARM = false;
bool loadDiffuse = false;
bool launchmaterialeditor = false;

int winwidth;
int winheight;

int page = 0;
int gsmpage = 0;
int graphicsquality = 2;

float farplane = 100;
float gravity = 4.0f;

char name[256];
char diffusepath[256];
char normalpath[256];
char armpath[256];
char savepath[256];
char savename[256];
char objname[256];
char matname[256] = "Material";

int minx = -2;

float blurstrength;

unsigned int guiloopnumber = 0;

std::string projectname;
std::string outprojectname;
std::string deletefile;

std::vector<std::string> filenames;
std::vector<std::string> copiedfilenames;
std::vector<std::string> available_projects;
std::vector<Object> copiedobjects;
std::vector<Material> Materials;
std::vector<int> loopedobj;
std::vector<int> loopingobj;

ImGui::FileBrowser Filething;
ImGui::FileBrowser Materialbrowser;

Material newmat;

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
                    lodproj = true;
                    starteditor = true;
                    outprojectname = projectname;
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
            if (ImGui::Checkbox("Use Vulkan Renderer", &usevulkan)) {
                std::cout << "WARNING USING UNSTABLE VULKAN RENDERER\n";
                std::cout << "Risks may include:\n";
                std::cout << "Primitive Renderer\n";
                std::cout << "Updates coming late\n";
                std::cout << "Code completely breaking\n";
                std::cout << "Computer Crashes\n\n";
            }
            if (usevulkan == true) {
                ImGui::Text("The Current Vulkan Renderer is not functional in this version.\nIt'll Just crash!");
                ImGui::Text("WARNING VULKAN RENDERER IS UNSTABLE");
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

    Filething.SetTitle("Select A File");
    Materialbrowser.SetTitle("Select a material file");
    Materialbrowser.SetTypeFilters({".png", ".jpg", ".jpeg", ".webp"});
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

void GUI::Update(GLFWwindow* window, Camera& cam) {
    //provide the whole background for ImGUI to use
    ImGui::DockSpaceOverViewport();

    bool objectselected = false;

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::BeginMenu("Save")) {
                ImGui::InputText("Project Name", savename, 256);
                if (ImGui::Button("Save")) {
                    std::cout << "DEBUG: About to call SaveProject with " << filenames.size() << " files." << std::endl;
                    SaveProject(savename, filenames, cam);
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
                            SceneLights.clear(); //doesn't work??
                            Materials.clear();
                            LoadProject(projectname.c_str(), filenames, cam);
                        }
                    }
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Object")) {
                    if (ImGui::Button("OBJ File")) {
                        Filething.SetTypeFilters({".obj"});
                        loadOBJ = true;
                        Filething.Open();
                    }
                    /*if (ImGui::Button("FBX File")) {
                        loadFBX = true;
                        std::cout << "LoadFBX == true\n";
                        Filething.SetTypeFilters({".fbx"});
                        Filething.Open();
                    }*/
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
        if (ImGui::BeginMenu("Test")) {
            if (ImGui::Button("Lauch in editor")) {
                for (int i = 0; i < Objects.size(); i++) {
                    Object& obj = Objects[i];
                    obj.id = i;
                    AABB(obj);
                }
                BVHtree = BuildBVH(Objects);
                launchie = true;
            }
            if (ImGui::Button("Stop Playing")) {
                launchie = false;
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Add")) {
            if (ImGui::BeginMenu("Object")) {
                if (ImGui::Button("OBJ File")) {
                    Filething.SetTypeFilters({".obj"});
                    loadOBJ = true;
                    Filething.Open();
                }
                /*if (ImGui::Button("FBX File")) {
                    loadFBX = true;
                    std::cout << "LoadFBX == true\n";
                    Filething.SetTypeFilters({".fbx"});
                    Filething.Open();
                }*/
                ImGui::Text("More File Formats Coming Soon!");
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Light")) {
                if (ImGui::Button("Add Point Light")) {
                    Light newlight;
                    newlight.name = "Light" + std::to_string(SceneLights.size());
                    SceneLights.push_back(newlight);
                }
                ImGui::EndMenu();
            }
            if (ImGui::Button("Material")) {
                launchmaterialeditor = true;
                //go to line 780 for implementation of material editor
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
                if (ImGui::Button("Stats")) {
                    gsmpage = 3;
                }
                if (ImGui::Button("Game Settings")) {
                    gsmpage = 4;
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
                    InitializeUniforms();
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
                ImGui::Separator();
                //ImGui::DragFloat("Shadow Far Plane", &farplane, 1.0f, 10, 1000); Eh I'll make it later.. But for now it's simpler to just not :3
                if (graphicsquality < 2) {
                    ImGui::Separator;
                    ImGui::Text("Blur Settings");
                    if (ImGui::BeginCombo("Blur Sample Size", "Blur Sample Size")) {
                        if (ImGui::Button("3x3 grid")) {
                            minx = -1;
                            blurstrength = 0.33;
                            calculategaussian(blurstrength, minx);
                        }
                        if (ImGui::Button("5x5 grid")) {
                            minx = -2;
                            blurstrength = 0.67;
                            calculategaussian(blurstrength, minx);
                        }
                        if (ImGui::Button("7x7 grid")) {
                            minx = -3;
                            blurstrength = 1;
                            calculategaussian(blurstrength, minx);
                        }
                        if (ImGui::Button("9x9 grid")) {
                            minx = -4;
                            blurstrength = 1.33;
                            calculategaussian(blurstrength, minx);
                        }
                        ImGui::EndCombo();
                    }
                }
                if (ImGui::Button("Apply Graphics Settings")) {
                    Applysettings();
                }
                if (ImGui::Button("Back")) {
                    gsmpage = 0;
                }
            }
            if (gsmpage == 3) {
                ImGui::Text("Renderer FPS");
                ImGui::Text("Render Loop FPS: %.2f", FPS);
                ImGui::Separator();
                ImGui::Text("Active CameraPos");
                ImGui::Text("X: %.2f, Y: %.2f, Z: %.2f", cam.position.x, cam.position.y, cam.position.z);
                if (ImGui::Button("Back")) {
                    gsmpage = 0;
                }
            }
            if (gsmpage == 4) {
                ImGui::Text("Camera Settings");
                ImGui::SliderFloat("Camera Speed", &playerspeed, 0.5f, 20.0f);
                ImGui::Separator();
                ImGui::Text("Player Settings");
                if (ImGui::BeginCombo("Player Collision Size", "Player Collision Size")) {
                    ImGui::SliderFloat("Player Height", &playerheight, 0.5f, 100.0f);
                    ImGui::SliderFloat("Player Width", &playerwidth, 0.5f, 100.0f);
                    ImGui::SliderFloat("Player Depth", &playerdepth, 0.5f, 100.0f);
                    if (ImGui::Button("Apply")) {
                        playercolcalc();
                    }
                    ImGui::EndCombo();
                }
                ImGui::SliderFloat("Gravity", &gravity, 0.5f, 20.0f);
                ImGui::Text("More coming soon");
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
        //What actually get's the render!
        ImGui::Image((void*)(intptr_t)actualFinalTexture, viewportSize, ImVec2(0,1), ImVec2(1, 0));
        ImGui::End();
    }
    if (ImGui::Begin("Editor")) {
        if (ImGui::BeginTabBar("Editor")) {
            if (ImGui::BeginTabItem("Objects")) {
                ImGui::Text("New");
                ImGui::InputText("Add New Obj", objfile, 256);
                const char* combo_id = "AddCombo";
                static const char* preview = "Add";
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

                    glm::vec3 color = glm::vec3(1.0f);
                    glm::vec3 ogcolor = glm::vec3(1.0f);
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
                    activeobj.name = objname;
                    ImGui::Text("Transform Controls");
                    ImGui::DragFloat3("Position", &activeobj.position.x, 0.1f);
                    ImGui::DragFloat3("Rotation", &activeobj.rotation.x, 0.1f);
                    ImGui::DragFloat3("Scale", &activeobj.scale.x, 0.1f);
                    ImGui::Text("Color/Texture");
                    ImGui::ColorEdit3("Base Color", &color.x);
                    if (Materials.size() != 0) {
                        if (ImGui::BeginCombo("Material", Materials[activeobj.materialid].name.c_str())) {
                            for (int j = 0; j < Materials.size(); j++) {
                                ImGui::PushID(j);
                                if (ImGui::Selectable(Materials[j].name.c_str())) {
                                    activeobj.diffuseMap = Materials[j].diffuseMap;
                                    activeobj.normalMap = Materials[j].normalMap;
                                    activeobj.armMap = Materials[j].armMap;
                                    activeobj.materialid = Materials[j].id;
                                }
                                ImGui::PopID();
                            }
                            ImGui::EndCombo();
                        }
                    } else {
                        ImGui::NewLine();
                        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), " Looks like you haven't\n made materials yet!\n go to the material editor to\n change that!");
                        ImGui::NewLine();
                        //ooh colorful thanks google for telling me how to do that
                    }
                    if (ImGui::Button("Delete Object")) {
                        deleteobjects = true;
                        Objects.erase(Objects.begin() + activeobj.id);
                        filenames.erase(filenames.begin() + activeobj.id);
                    }
                    position = activeobj.position - originalpos;
                    //scale -= activeobj.scale - originalscale;
                    rotation -= activeobj.rotation - originalrotation;
                    for (int i = Objects.size() - 1; i > -1; i--) {
                        Object& obj = Objects[i];
                        if (obj.selected && i != activeobject) {
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
                    copiedobjects[i].id = Objects.size() - 1;
                    Objects.push_back(copiedobjects[i]);
                    filenames.push_back(copiedfilenames[i]);
                    ispasted = true;
                }
            }
        }
    } else {
        ispasted = false;
    }

    if (launchmaterialeditor == true) {
        if (ImGui::Begin("Material Editor")) {
            ImGui::InputText("Name", matname, sizeof(matname));
            newmat.name = std::string(matname);
            ImGui::NewLine();
            ImGui::Text("Browse");
            if (ImGui::Button("Albedo")) {
                Materialbrowser.Open();
                loadDiffuse = true;
            }
            if (ImGui::Button("Normals")) {
                Materialbrowser.Open();
                loadNormals = true;
            }
            if (ImGui::Button("ARM")) {
                Materialbrowser.Open();
                loadARM = true;
            }
            ImGui::NewLine();
            ImGui::Separator();
            ImGui::NewLine();
            if (ImGui::Button("Load Texture")) {
                Materials.push_back(newmat);
            }
            ImGui::Separator();
            if (ImGui::Button("Close Material Editor")) {
                launchmaterialeditor = false;
            }
            ImGui::End();
        }
    }

    Filething.Display(); //display the filesystem if it opened

    if (Filething.HasSelected()) {
        std::cout << "Detected Choice on Object\n";
        if (loadOBJ == true) {
            std::string path = Filething.GetSelected().string();
            std::cout << "Loading File: " << path << std::endl;
            positions.clear();
            indices.clear();
            uvs.clear();
            normals.clear();
            tangents.clear();

            Hmodel.name = "Object" + std::to_string(Objects.size());

            std::cout << "loading object vertices with objload\n" << std::endl;
            Hmodel.vertexCount = objload(path.c_str(), positions, indices, uvs, normals, tangents);
            Hmodel.vertices = positions;
            Hmodel.indices = indices;
            Hmodel.objtangents = tangents;
            Hmodel.uvs = uvs;
            Hmodel.normals = normals;
            Hmodel.color = objectcolor;
            Hmodel.id = Objects.size();

            filenames.push_back(std::string(objfile));

            std::cout << "Loading Object with OBJ\n" << std::endl;
            CreateObjectWithOBJ(Hmodel);
            std::cout << "Done Loading Object with OBJ\n";

            Objects.push_back(Hmodel);

            std::cout << "DEBUG: Running UpdateBuffers\n";

            UpdateBuffers(Objects.back());

            std::cout << "DEBUG: RUNNING AABB\n";
            AABB(Objects.back());
            std::cout << "Successfull! :D" << std::endl;
            loadOBJ = false;
        }
        if (loadFBX == true) {
            std::cout << "Attempting to load FBX File ";
            std::string path = Filething.GetSelected().string();
            fbxload(path.c_str());
            loadFBX = false;
        }
        Filething.ClearSelected();
    }

    Materialbrowser.Display();
    if (Materialbrowser.HasSelected()) {
        if (loadARM == true) {
            std::string path = Materialbrowser.GetSelected().string();
            std::cout << "ARM Path: " << path << std::endl;
            newmat.armMap = LoadTextures(path.c_str(), false);
            memcpy(newmat.armpath, path.c_str(), sizeof(newmat.armpath));
            loadARM = false;
        }
        if (loadNormals == true) {
            std::string path = Materialbrowser.GetSelected().string();
            std::cout << "Normal Path: " << path << std::endl;
            newmat.normalMap = LoadTextures(path.c_str(), false);
            memcpy(newmat.normalpath, path.c_str(), sizeof(newmat.normalpath));
            loadNormals = false;
        }
        if (loadDiffuse == true) {
            std::string path = Materialbrowser.GetSelected().string();
            std::cout << "Diffuse Path: " << path << std::endl;
            newmat.diffuseMap = LoadTextures(path.c_str(), true);
            memcpy(newmat.diffusepath, path.c_str(), sizeof(newmat.diffusepath));
            loadDiffuse = false;
        }

        newmat.id = Materials.size();

        Materialbrowser.ClearSelected();
    }

    waspasted = ispasted;
    wascopied = iscopied;
    guiloopnumber++;
}
