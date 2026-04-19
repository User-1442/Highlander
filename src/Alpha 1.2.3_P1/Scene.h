#pragma once
#include <vector>
#include "glm/glm.hpp"
#include <string>

struct Object {
    std::string objtype;

    std::string name;

    bool selected = false;

    unsigned int VAO;
    unsigned int VBO;
    unsigned int colorBuffer;
    unsigned int normalBuffer;
    unsigned int tangentbuffer;
    unsigned int EBO;

    // Where the data is storeddddd :D
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;
    std::vector<glm::vec3> objtangents;
    glm::vec3 color;
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);

    GLuint diffuseMap;
    GLuint normalMap;
    GLuint armMap;

    char diffusepath[256] = "\0";
    char normalpath[256] = "\0";
    char armpath[256] = "\0";

    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);

    unsigned int vertexCount;
};

struct Light {
    std::string name;
    glm::vec3 position;
    glm::vec3 color;
    float brightness;
};

extern std::vector<Light> SceneLights;
extern std::vector<Object> Objects;
