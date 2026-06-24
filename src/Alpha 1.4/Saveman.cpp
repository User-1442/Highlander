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
bool needmakedir = true;
bool beforeAlpha14 = false;

bool nocoldef = true;

Material savematerial;

int SaveProject(
    char* projectname,
    std::vector<std::string>& filenames,
    Camera& cam
) {

    std::cout << "Attempting to save \n";
    success = true;
    // Creates the initial files :D

    if (fs::is_directory(projectname) == true) {
        needmakedir = false;
    } else {
        needmakedir = true;
    }

    if (needmakedir == true) {
        if (fs::create_directory(projectname)) {
            std::cout << "Project File directory created!\n";
            if (fs::create_directory(std::string(projectname) + "/" + std::string("Materials"))) {
                std::cout << "Material Directory Created!\n";
            } else {
                std::cout << "ERROR: FAILED TO CREATE MATERIAL FILE DIRECTORY\n";
            }
        } else {
            std::cout << "ERROR: FAILED TO CREATE FILE DIRECTORY\n";
            success = false;
        }
    }

    ofstream Project(std::string("./") + projectname + "/" + projectname + ".Highlander", std::ios::trunc);

    Project << "Highlander TBSS 1.2\n";

    for (int i = 0; i < Materials.size(); i++) {
        std::cout << "Attempting to save: " << Materials[i].name << "\n" << std::endl;
        std::cout << "Saving diffuse: " << Materials[i].diffusepath << "\n" << std::endl;
        std::cout << "Saving Normals: " << Materials[i].normalpath << "\n" << std::endl;
        std::cout << "Saving ARM: " << Materials[i].armpath << "\n" << std::endl;
        fs::path matdifpath(Materials[i].diffusepath);
        fs::path matnorpath(Materials[i].normalpath);
        fs::path matarmpath(Materials[i].armpath);
        std::string newdifpath = matdifpath.filename().string();
        std::string newnorpath = matnorpath.filename().string();
        std::string newarmpath = matarmpath.filename().string();
        if (fs::exists(std::string("./") + projectname + std::string("/Materials") + "/")) {
            if (fs::exists(std::string("./") + projectname + "/" + std::string("Materials") + "/" + std::string(newdifpath)) == false) {
                fs::copy(std::string(Materials[i].diffusepath), std::string("./") + projectname + "/" + std::string("Materials") + "/", fs::copy_options::overwrite_existing);
            } else {
                std::cout << "WARNING::FILE::ALREADY::EXISTS::SKIPPING << MAY::CAUSE::UNEXPECTED::EFFECTS\n";
            }
            if (fs::exists(std::string("./") + projectname + "/" + std::string("Materials") + "/" + std::string(newnorpath)) == false) {
                fs::copy(std::string(Materials[i].normalpath), std::string("./") + projectname + "/" + std::string("Materials") + "/", fs::copy_options::overwrite_existing);
            } else {
                std::cout << "WARNING::FILE::ALREADY::EXISTS::SKIPPING << MAY::CAUSE::UNEXPECTED::EFFECTS\n";
            }
            if (fs::exists(std::string("./") + projectname + "/" + std::string("Materials") + "/" + std::string(newarmpath)) == false) {
                fs::copy(std::string(Materials[i].armpath), std::string("./") + projectname + "/" + std::string("Materials") + "/", fs::copy_options::overwrite_existing);
            } else {
                std::cout << "WARNING::FILE::ALREADY::EXISTS::SKIPPING << MAY::CAUSE::UNEXPECTED::EFFECTS\n";
            }
        }
        Project << std::string("-Matname ") + Materials[i].name + "\n";
        Project << std::string("-Matdif ") + newdifpath + "\n";
        Project << std::string("-Matnor ") + newnorpath + "\n";
        Project << std::string("-Matarm ") + newarmpath + "\n";
    }

    for (int i = 0; i < Objects.size(); i++) {
        Object& obj = Objects[i];
        bool alreadyexists = false;
        fs::path ogobjectpath(filenames[i]);
        if (fs::exists(std::string("./") + projectname + "/" + ogobjectpath.filename().string())) {
            std::cout << "WARNING::REPEATED::FILENAMES::MAY::CAUSE::UNEXPECTED::EFFECTS\n";
        } else {
            fs::copy(/*std::string(ogobjectpath)*/ogobjectpath.string(), std::string("./") + projectname + "/", fs::copy_options::overwrite_existing);
        }
        std::string newobjectpath = ogobjectpath.filename().string();
        Project << "-Obj " + newobjectpath + "\n";
        if (!obj.name.empty()) {
            Project << std::string("-Oname " + obj.name + "\n");
        } else {
            Project << std::string(std::string("-Oname ") + "Object[" + std::to_string(i) + "]\n");
        }
        Project << std::string("-Omatid ") + std::to_string(obj.materialid + 1) + "\n";
        Project << std::string("-Ocol ") + std::to_string(obj.color.x) + " " + std::to_string(obj.color.y) + " " + std::to_string(obj.color.z) + "\n";
        /*if (obj.diffusepath[0] != '\0') {
            fs::path ogdiffusepath(obj.diffusepath);
            if (fs::exists(std::string("./") + projectname + "/" + ogdiffusepath.filename().string()) == false) {
                fs::copy(std::string(obj.diffusepath), std::string("./") + projectname + "/", fs::copy_options::overwrite_existing);
            }
            std::string newdiffusepath = ogdiffusepath.filename().string();
            Project << std::string("-Odif ") + newdiffusepath + "\n";
        }
        if (obj.normalpath[0] != '\0') {
            fs::path ognormalpath(obj.normalpath);
            std::string newnormalpath = ognormalpath.filename().string();
            if (fs::exists(std::string("./") + projectname + "/" + ognormalpath.filename().string()) == false) {
                fs::copy(std::string(obj.normalpath), std::string("./") + projectname + "/", fs::copy_options::overwrite_existing);
            }
            Project << std::string("-Onor ") + newnormalpath + "\n"; //OR NORRRRR
        }
        if (obj.armpath[0] != '\0') {
            fs::path ogarmpath(obj.armpath);
            std::string newarmpath = ogarmpath.filename().string();
            if (fs::exists(std::string("./") + projectname + "/" + ogarmpath.filename().string()) == false) {
                fs::copy(std::string(obj.armpath), std::string("./") + projectname + "/", fs::copy_options::overwrite_existing);
            }
            Project << std::string("-Oarm ") + newarmpath + "\n";
        }*/
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
    Project << std::string("-Camrot ") + std::to_string(cam.direction.x) + " " + std::to_string(cam.direction.y) + " " + std::to_string(cam.direction.z) + "\n";
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
            if (prefix == "-Matname") {
                ss >> savematerial.name;
            }
            if (prefix == "-Matdif") { //new material system and therefore new changes to the save system in Alpha 1.4!
                std::string materialloc;
                ss >> materialloc;
                materialloc = std::string("./") + projectname + std::string("/Materials") + "/" + materialloc;
                char materialocation[256];
                std::strncpy(materialocation, materialloc.c_str(), sizeof(materialocation) - 1);
                printf(materialocation);
                savematerial.diffuseMap = LoadTextures(materialocation, true);
                memcpy(savematerial.diffusepath, materialocation, sizeof(savematerial.diffusepath));
            }
            if (prefix == "-Matnor") {
                std::string materialloc;
                ss >> materialloc;
                materialloc = std::string("./") + projectname + std::string("/Materials") + "/" + materialloc;
                char materialocation[256];
                std::strncpy(materialocation, materialloc.c_str(), sizeof(materialocation) - 1);
                printf(materialocation);
                savematerial.normalMap = LoadTextures(materialocation, false);
                memcpy(savematerial.normalpath, materialocation, sizeof(savematerial.normalpath));
            }
            if (prefix == "-Matarm") {
                std::string materialloc;
                ss >> materialloc;
                materialloc = std::string("./") + projectname + std::string("/Materials") + "/" + materialloc;
                char materialocation[256];
                std::strncpy(materialocation, materialloc.c_str(), sizeof(materialocation) - 1);
                printf(materialocation);
                savematerial.armMap = LoadTextures(materialocation, false);
                memcpy(savematerial.armpath, materialocation, sizeof(savematerial.armpath));
                Materials.push_back(savematerial);
            }
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
                savematerial.diffuseMap = Newobj.diffuseMap;
                beforeAlpha14 = true;
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
                savematerial.normalMap = Newobj.normalMap;
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
                savematerial.armMap = Newobj.armMap; //makes sure saved projects before Alpha 1.4 can still be saved in newer versions.
                savematerial.id = Materials.size();
                savematerial.name = Newobj.name + std::string("Material");
                memcpy(savematerial.diffusepath, Newobj.diffusepath, sizeof(savematerial.diffusepath));
                memcpy(savematerial.normalpath, Newobj.normalpath, sizeof(savematerial.normalpath));
                memcpy(savematerial.armpath, Newobj.armpath, sizeof(savematerial.armpath));
                Newobj.materialid = savematerial.id;
                Materials.push_back(savematerial);
            }
            if (prefix == "-Omatid") {
                Object& Newobj = Objects.back();
                beforeAlpha14 = false;
                ss >> Newobj.materialid;
                Newobj.materialid -= 1; //since vectors start at 0 and the id starts at 1, you need to make sure that the unsigned int can start at 0 for the id.
                Newobj.diffuseMap = Materials[Newobj.materialid].diffuseMap;
                Newobj.normalMap = Materials[Newobj.materialid].normalMap;
                Newobj.armMap = Materials[Newobj.materialid].armMap;
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
            if (prefix == "-Camrot") {
                ss >> cam.direction.x >> cam.direction.y >> cam.direction.z;
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
        Modelparser();
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

