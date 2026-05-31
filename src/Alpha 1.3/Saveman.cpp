#include "glad.h"
#include "Saveman.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <filesystem>
#include "Scene.h"
#include "glm/glm.hpp"
#include "sstream"
#include "objloader.h"
#include "renman.h"
#include <cstring>
#include "Camera.h"
#include "collisionman.h"

namespace fs = std::filesystem;
using namespace std;
using namespace std::string_literals;

bool success;

bool nocoldef = true;

int SaveProject(
    char* projectname,
    std::vector<std::string>& filenames,
    Camera& cam
) {

    std::cout << "Attempting to save \n";
    success = true;
    // Creates the initial files :D

    if (fs::is_directory(projectname) == true) {
        std::filesystem::remove_all(projectname);
    }

    if (fs::create_directory(projectname)) {
        std::cout << "Project File directory created!";
    } else {
        std::cout << "ERROR: FAILED TO CREATE FILE DIRECTORY";
        success = false;
    }

    ofstream Project(std::string("./") + projectname + "/" + projectname + ".Highlander");

    Project << "Highlander\n";

    for (int i = 0; i < Objects.size(); i++) {
        Object& obj = Objects[i];
        fs::path ogobjectpath(filenames[i]);
        fs::copy(/*std::string(ogobjectpath)*/ogobjectpath.string(), std::string("./") + projectname + "/", fs::copy_options::overwrite_existing);
        std::string newobjectpath = ogobjectpath.filename().string();
        Project << "-Obj " + newobjectpath + "\n";
        if (!obj.name.empty()) {
            Project << std::string("-Oname " + obj.name + "\n");
        } else {
            Project << std::string(std::string("-Oname ") + "Object[" + std::to_string(i) + "]\n");
        }
        Project << std::string("-Ocol ") + std::to_string(obj.color.x) + " " + std::to_string(obj.color.y) + " " + std::to_string(obj.color.z) + "\n";
        if (obj.diffusepath[0] != '\0') {
            fs::path ogdiffusepath(obj.diffusepath);
            fs::copy(std::string(obj.diffusepath), std::string("./") + projectname + "/", fs::copy_options::overwrite_existing);
            std::string newdiffusepath = ogdiffusepath.filename().string();
            Project << std::string("-Odif ") + newdiffusepath + "\n";
        }
        if (obj.normalpath[0] != '\0') {
            fs::path ognormalpath(obj.normalpath);
            fs::copy(std::string(obj.normalpath), std::string("./") + projectname + "/", fs::copy_options::overwrite_existing);
            std::string newnormalpath = ognormalpath.filename().string();
            Project << std::string("-Onor ") + newnormalpath + "\n"; //OR NORRRRR
        }
        if (obj.armpath[0] != '\0') {
            fs::path ogarmpath(obj.armpath);
            fs::copy(std::string(obj.armpath), std::string("./") + projectname + "/", fs::copy_options::overwrite_existing);
            std::string newarmpath = ogarmpath.filename().string();
            Project << std::string("-Oarm ") + newarmpath + "\n";
        }
        Project << std::string("-Oloc ") + std::to_string(obj.position.x) + " " + std::to_string(obj.position.y) + " " + std::to_string(obj.position.z) + "\n";
        Project << std::string("-Orot ") + std::to_string(obj.rotation.x) + " " + std::to_string(obj.rotation.y) + " " + std::to_string(obj.rotation.z) + "\n";
        Project << std::string("-Osca ") + std::to_string(obj.scale.x) + " " + std::to_string(obj.scale.y) + " " + std::to_string(obj.scale.z) + "\n";
        if (obj.colmincoords != obj.colmaxcoords) {
            Project << std::string("-Ocolmin ") + std::to_string(obj.colmincoords.x) + " " + std::to_string(obj.colmincoords.y) + " " + std::to_string(obj.colmincoords.z) + "\n";
            Project << std::string("-Ocolmax ") + std::to_string(obj.colmaxcoords.x) + " " + std::to_string(obj.colmaxcoords.y) + " " + std::to_string(obj.colmaxcoords.z) + "\n";
            Project << std::string("-Ocolen ") + std::to_string(obj.collision) + "\n";
        }
    }

    Project << std::string("-OLE\n"); // stands for Object List End

    for (int i = 0; i < SceneLights.size(); i++) {
        Light& light = SceneLights[i];
        Project << std::string("-PLight") + "\n";
        if (!light.name.empty()) {
            Project << std::string("-Plnam ") + light.name + "\n";
        }
        Project << std::string("-Plbri ") + std::to_string(light.brightness) + "\n";
        Project << std::string("-Plcol ") + std::to_string(light.color.x) + " " + std::to_string(light.color.y) + " " + std::to_string(light.color.z) + "\n";
        Project << std::string("-Plloc ") + std::to_string(light.position.x) + " " + std::to_string(light.position.y) + " " + std::to_string(light.position.z) + "\n";
    }
    Project << std::string("-Campos ") + std::to_string(cam.position.x) + " " + std::to_string(cam.position.y) + " " + std::to_string(cam.position.z) + "\n";
    Project << std::string("-EOF");

    Project.close();

    if (success == false) {
        return -1;
    } else {
        std::cout << "\n01110100 01101000 01100001 01101110 01101011 00100000 01111001 01101111 01110101 aka: It worked!\n";
        return 0;
    }
}

int LoadProject(
    const std::string& projectname,
    std::vector<std::string>& filenames,
    Camera& cam
) {
    std::cout << "Opened Loading Manager\n";
    std::ifstream Project(std::string(projectname) + "/" + std::string(projectname) + ".Highlander");
    if (Project.is_open()) {
        filenames.clear();
        Objects.clear();
        std::string line;
        std::cout << "Highlander Project Loading!\n";
        int objcout = 0;
        int plcout = 0;
        while (std::getline(Project, line)) {
            std::stringstream ss(line);
            std::string prefix;
            ss >> prefix;
            if (prefix == "-Obj") {
                Object Newobj;
                std::string objstring;
                ss >> objstring;
                char objfile[256];
                objstring = std::string("./") + projectname + "/" + objstring;
                std::strncpy(objfile, objstring.c_str(), sizeof(objfile) - 1);
                positions.clear();
                indices.clear();
                tangents.clear();
                positions.clear();
                indices.clear();
                uvs.clear();
                normals.clear();
                tangents.clear();
                printf("Loading OBJ file: ");
                printf(objfile);
                printf("\n");
                Newobj.vertexCount = objload(objfile, positions, indices, uvs, normals, tangents);
                Newobj.vertices = positions;
                Newobj.indices = indices;
                Newobj.objtangents = tangents;
                Newobj.uvs = uvs;
                Newobj.normals = normals;
                Objects.push_back(Newobj);
                filenames.push_back(objstring);
                objcout ++;
                Newobj.id = objcout;
            }
            if (prefix == "-Oname") {
                std::cout << "Loading Obj Name\n";
                Object& Newobj = Objects.back();
                ss >> Newobj.name;
            }
            if (prefix == "-Ocol") {
                std::cout << "Loading OBJ base color\n";
                Object& Newobj = Objects.back();
                glm::vec3 color;
                ss >> Newobj.color.x >> Newobj.color.y >> Newobj.color.z;
            }
            if (prefix == "-Odif") {
                std::cout << "Loading diffuse\n";
                Object& Newobj = Objects.back();
                std::string diffusepathstring;
                ss >> diffusepathstring;
                diffusepathstring = /*std::string("./") + */projectname + "/" + diffusepathstring;
                std::strncpy(Newobj.diffusepath, diffusepathstring.c_str(), sizeof(Newobj.diffusepath));
                Newobj.diffuseMap = LoadTextures(Newobj.diffusepath, true);
            }
            if (prefix == "-Onor") {
                std::cout << "Loading normals\n";
                Object& Newobj = Objects.back();
                std::string normalpathstring;
                ss >> normalpathstring;
                normalpathstring = /*std::string("./") + */projectname + "/" + normalpathstring;
                std::strncpy(Newobj.normalpath, normalpathstring.c_str(), sizeof(Newobj.normalpath));
                Newobj.normalMap = LoadTextures(Newobj.normalpath, false);
                std::cout << "Normals Loaded: ";
                std::cout << Newobj.normalpath;
                std::cout << "\n";
            }
            if (prefix == "-Oarm") {
                Object& Newobj = Objects.back();
                std::string armpathstring;
                ss >> armpathstring;
                armpathstring = /*std::string("./") + */projectname + "/" + armpathstring;
                std::strncpy(Newobj.armpath, armpathstring.c_str(), sizeof(Newobj.armpath));
                Newobj.armMap = LoadTextures(Newobj.armpath, false);
                std::cout << "ARM Loaded: ";
                std::cout << Newobj.armpath;
                std::cout << "\n";
            }
            if (prefix == "-Oloc") {
                Object& Newobj = Objects.back();
                ss >> Newobj.position.x >> Newobj.position.y >> Newobj.position.z;
            }
            if (prefix == "-Orot") {
                Object& Newobj = Objects.back();
                ss >> Newobj.rotation.x >> Newobj.rotation.y >> Newobj.rotation.z;
            }
            if (prefix == "-Osca") {
                Object& Newobj = Objects.back();
                ss >> Newobj.scale.x >> Newobj.scale.y >> Newobj.scale.z;
            }
            if (prefix == "-Ocolmin") {
                Object& Newobj = Objects.back();
                ss >> Newobj.colmincoords.x >> Newobj.colmincoords.y >> Newobj.colmincoords.z;
            }
            if (prefix == "-Ocolmax") {
                Object& Newobj = Objects.back();
                ss >> Newobj.colmaxcoords.x >> Newobj.colmaxcoords.y >> Newobj.colmaxcoords.z;
            }
            if (prefix == "-Ocolen") {
                Object& Newobj = Objects.back();
                ss >> Newobj.collision;
                nocoldef = false;
            }
            if (prefix == "-PLight") {
                Light Newlight;
                SceneLights.push_back(Newlight);
                plcout++;
            }
            if (prefix == "-Plnam") {
                Light& Newlight = SceneLights.back();
                ss >> Newlight.name;
            }
            if (prefix == "-Plbri") {
                Light& Newlight = SceneLights.back();
                ss >> Newlight.brightness;
            }
            if (prefix == "-Plcol") {
                Light& Newlight = SceneLights.back();
                ss >> Newlight.color.x >> Newlight.color.y >> Newlight.color.z;
            }
            if (prefix == "-Plloc") {
                Light& Newlight = SceneLights.back();
                ss >> Newlight.position.x >> Newlight.position.y >> Newlight.position.z;
            }
            if (prefix == "-Campos") {
                ss >> cam.position.x >> cam.position.y >> cam.position.z;
            }
            if (prefix == "-EOF") {
                std::cout << "File ended!";
                size_t vector_size = Objects.size();
                for (int i = 0; i < objcout; i++) {
                    Object& Newobj = Objects[vector_size - (objcout - i)];
                    CreateObjectWithOBJ(Newobj);
                    UpdateBuffers(Newobj);
                }
                for (int i = 0; i < Objects.size(); i++) {
                    Objects[i].id = i;
                }
                for (int i = 0; i < plcout; i++) {
                    Light& Newlight = SceneLights.back();
                    InitUniforms();
                }
                success = true;
            }
        }
    }
    if (nocoldef == true && success == true) {
        std::cout << "Calculating Collision Bounding Box\n";
        for (int i = 0; i < Objects.size(); i++) {
            AABB(Objects[i]);
        }
    }
    if (success = true) {
        std::cout << "Closing Project! \n";
        Project.close();
        return 0;
    } else {
        std::cout << "Dang bro your thing brokie... Imma close it anyway but that's sad.";
        Project.close();
        return -1;
    }
}

