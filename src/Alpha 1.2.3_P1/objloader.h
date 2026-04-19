#pragma once
#include "glm/glm.hpp"
#include <vector>

extern std::vector<glm::vec3> positions;
extern std::vector<unsigned int> indices;
extern std::vector<glm::vec2> uvs;
extern std::vector<glm::vec3> normals;

// Function to load the OBJ file and return the total number of vertices to draw
int objload(
    const char* filename,
    std::vector<glm::vec3>& out_positions,
    std::vector<unsigned int>& out_indices,
    std::vector<glm::vec2>& out_uvs,
    std::vector<glm::vec3>& out_normals,
    std::vector<glm::vec3>& out_tangents
);
//new function with the EBO variables :D
