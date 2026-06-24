#pragma once
#include <fstream>
#include <iostream>
#include <vector>
#include "Scene.h"
#include "Camera.h"
using namespace std;

int SaveProject(
    char* projectname,
    std::vector<std::string>& filenames,
    Camera& cam
);

int LoadProject(const std::string& projectname, std::vector<std::string>& filenames, Camera& cam);
