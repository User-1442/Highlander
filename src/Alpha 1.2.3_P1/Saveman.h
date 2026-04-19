#pragma once
#include <fstream>
#include <iostream>
#include <vector>
#include "Scene.h"
using namespace std;

int SaveProject(
    char* projectname,
    std::vector<std::string>& filenames
);

int LoadProject(const std::string& projectname, std::vector<std::string>& filenames);
