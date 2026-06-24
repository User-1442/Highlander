#define FOURCC_DXT1 0x31545844 // "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // "DXT3"
#define FOURCC_DXT5 0x35545844 // "DXT5"
#define GLM_ENABLE_EXPERIMENTAL
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "glad.h"
#include <stdio.h>
#include <stdlib.h>
#include "glfw3.h"
#include "renman.h"
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Shaderlogic.hpp"
#include <cstring>
#include "objloader.h"
#include "Camera.h"
#include "Scene.h"
#include "GUI.h"
#include <vector>
#include <cmath>
#include "vulkan/vulkan.hpp"
#include "vulkan/vulkan_raii.hpp"

#ifndef GL_COMPRESSED_RGBA_S3TC_DXT1_EXT
#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT 0x83F1
#endif

#ifndef GL_COMPRESSED_RGBA_S3TC_DXT3_EXT
#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT 0x83F2
#endif

#ifndef GL_COMPRESSED_RGBA_S3TC_DXT5_EXT
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT 0x83F3
#endif

/* NOTE FOR ME:
A NORMAL IS PERPENDICULAR TO THE MESH. THE LIGHT DIRECTION VECTOR AND THE NORMAL ARE NEEDED FOR BLINN PHONG LIGHTING

More notes will follow
*/

GLuint EBO;
GLuint HighVAO;
GLuint HighVBO;
GLuint View;
GLuint programID;
GLuint quadProgramID;
GLuint blurProgramID;
GLuint colorbuffer;
GLuint normalbuffer;
GLuint tangentbuffer;
GLuint MatrixID;
GLuint ModelMatrixID;
GLuint ViewMatrixID;
GLuint CamPosID;
GLuint SpecularID;
GLuint reflectivenessID;
GLuint AmbientLightID;
GLuint FBO;
GLuint depthrenderbuffer;
GLuint depthTexture;
GLuint quadVAO;
GLuint quadVBO;
GLuint renderedTexture;
GLuint meshcolorID;
GLuint numLightsID;
GLuint LightPosID;
GLuint LightColorID;
GLuint albedoID;
GLuint texturenormalID;
GLuint ARMID;
GLuint UBO;
GLuint finalFBO;
GLuint finalTexture;
GLuint projectionlocation;
GLuint gNormal;
GLuint gARM;
GLuint internalFormat;
GLuint dataFormat;
GLuint actualFinalTexture;
GLuint blurVAO;
GLuint blurVBO;
GLuint ARMWORLDtex;
GLuint Reflectionmask;
GLuint ggraphicsID;
GLuint SSRgraphicsID;
GLuint blurgraphicsID;
GLuint blurloc;
GLuint bluramount;
GLuint shadowperspectiveID;
GLuint shadowcubemapID;

GLint invprojectionv;
GLint projectionv;
GLint renderedTexturev;
GLint depthTexturev;
GLint gNormalv;
GLint gARMv;
GLint grainytexturev;
GLint gARMBlurv;
GLint reflectionmaskv;
GLint renderedTextureblurv;

glm::mat4 HighProjection;
glm::mat4 Model = glm::mat4(1.0f);
glm::mat4 previousmodel = glm::mat4(1.0f);
glm::mat4 camView;
glm::mat4 invProj;

double startTime;
double FPS;

bool UseTextures;

float blurstrengthb = 0.33;
int blurminx = -1;

std::vector<GLfloat> gaussian;
int loopnumber = 0;
int Windowwidth;
int Windowheight;
int TextureWidth;
int TextureHeight;
int TextureChannels;
int shadowcheck;
unsigned int attachments[3];
//IMPORTANT
float reflectiveness = 30;
char objfile[256] = "Highlander.obj";
unsigned char whitePixel[] = { 255, 255, 255, 255 };
unsigned char normalPixel[] = { 128, 128, 255, 255 };
struct ubostorage {
    glm::vec4 LightPos[500];
    glm::vec4 LightColor[500];
    unsigned int numLights;
    float padding[3];
};

glm::mat4 depthBiasMVP;
glm::mat4 biasMatrix( //multiplies everything by 0.5
0.5, 0.0, 0.0, 0.0,
0.0, 0.5, 0.0, 0.0,
0.0, 0.0, 0.5, 0.0,
0.5, 0.5, 0.5, 1.0
);

glm::vec3 ambientlight = glm::vec3(0.2, 0.2, 0.2);
glm::vec3 spec = glm::vec3(1, 1, 1);
glm::vec3 LightColor = glm::vec3(1, 1, 1);
glm::vec3 meshcolor = glm::vec3(0.0, 0.0, 0.5);

std::vector<glm::vec3> lightPositions;
std::vector<glm::vec3> lightColors;
std::vector<Light> SceneLights;
std::vector<glm::vec3> tangents;
std::vector<glm::mat4> Models;

//Used to make the texture on the screen. Creates 2 triangles that cover the screen.
float quadVertices[] = {
    // positions   // texCoords
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f, -1.0f,  1.0f, 0.0f,

    -1.0f,  1.0f,  0.0f, 1.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
     1.0f,  1.0f,  1.0f, 1.0f
};

//this is used to find the triangle count in the obj file
int g_vertexCount = 0;
float lastFrame = 0.0f;

void InitOpenGL(GLFWwindow* window, int width, int height) {
    stbi_set_flip_vertically_on_load(true);
    //Stride from 1 vertex to the next :3
    GLsizei stride = 4 * sizeof(float);
    glm::vec3 LightPosition = glm::vec3(0.0f, 0.0f, 2.0f); //light position.
    int g_vertexCount = objload(objfile, positions, indices, uvs, normals, tangents);
    std::cout << "Vertex Count: " << g_vertexCount << std::endl;
    std::cout << "sizeof(positions): " << float(sizeof(positions)) << std::endl;
    std::cout << "sizeof(uvs): " << float(sizeof(uvs)) << std::endl;
    std::cout << "sizeof(normals)" << float(sizeof(normals)) << std::endl;

    //MARK FOR REMOVAL: REMOVE THE PRINTF STUFF BELOW
    printf("First Vertex Position: (%.4f, %.4f, %.4f)\n",
       positions[0].x, positions[0].y, positions[0].z);

    // Print the first UV coordinate
    //printf("First UV: (%.4f, %.4f)\n",
        //uvs[0].x, uvs[0].y);

    for(int i=0; i < 6 && i < uvs.size(); i++) {
        std::cout << "UV[" << i << "]: (" << uvs[i].x << ", " << uvs[i].y << ")" << std::endl;
    }

    // Print the first Normal
    printf("First Normal: (%.4f, %.4f, %.4f)\n",
        normals[0].x, normals[0].y, normals[0].z);


    if (g_vertexCount <= 0) {
        printf("ERROR: Failed to load model or model is empty.\n");
        // You should probably exit or handle the error here but I'm too lazy so...
    }
    glGenVertexArrays(1, &HighVAO);
    glBindVertexArray(HighVAO);
    glGenBuffers(1, &HighVBO);

    glGenFramebuffers(1, &FBO);

    glBindBuffer(GL_ARRAY_BUFFER, HighVBO);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec3), &positions[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);


    glGenBuffers(1, &colorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(1);


    glGenBuffers(1, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(2);

    glGenBuffers(1, &tangentbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, tangentbuffer);glGetUniformLocation(quadProgramID, "gARM");
    glBufferData(GL_ARRAY_BUFFER, tangents.size() * sizeof(glm::vec3), &tangents[0], GL_STATIC_DRAW);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(3);

    glBindVertexArray(0);

    glBindVertexArray(HighVAO);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    //multiplies the number of tiles (count of unsigned int) and the size of 1 unsigned int (size of 1 tile)

    glGenBuffers(1, &UBO);
    glBindBuffer(GL_UNIFORM_BUFFER, UBO);

    glBufferData(GL_UNIFORM_BUFFER, sizeof(ubostorage), NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    //bind to address 0
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, UBO);

    std::cout << "VAO: " << HighVAO << std::endl;
    std::cout << "VBO: " << HighVBO << std::endl;

    programID = LoadShaders( "Vertex.glsl", "Fragment.glsl" );
    quadProgramID = LoadShaders("QuadV.glsl", "QuadF.glsl");
    blurProgramID = LoadShaders("Blur_Pass_V.glsl", "Blur_Pass_F.glsl");
    glUseProgram(programID);

    albedoID = glGetUniformLocation(programID, "albedoMap");
    texturenormalID = glGetUniformLocation(programID, "normalMap");
    ARMID = glGetUniformLocation(programID, "armMap");

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &whitePixel);

    glUniform1i(albedoID, 0);
    glUniform1i(texturenormalID, 1);
    glUniform1i(ARMID, 2);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);// Texture unit 0
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    //FBO TEXTURE LOGIC
    //This texture puts the entire scene into a 2D image texture and appends it to the FBO
    glGenTextures(1, &renderedTexture);
    glBindTexture(GL_TEXTURE_2D, renderedTexture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glGenTextures(1, &gARM);
    glBindTexture(GL_TEXTURE_2D, gARM);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderedTexture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gARM, 0);

    //make sure OpenGL uses all the color attachments
    unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);

    //Depth texture for SSR
    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);

    //define storage as DEPTH NOT RGB I SWEAR IT'LL BREAK THE SSR
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    // set the filtering for the depth
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

    /*Render Buffer Logic
    glGenRenderbuffers(1, &depthrenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);*/

    //input mode
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glBindVertexArray(HighVAO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderedTexture, 0);

    //Setting up Quad :D
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    //Position Attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);

    //Texture Coord Attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));

    glUseProgram(quadProgramID);
    glUniform1i(glGetUniformLocation(quadProgramID, "renderedTexture"), 0);
    glUniform1i(glGetUniformLocation(quadProgramID, "depthTexture"), 1);

    //THE THING THE THINGY
    projectionlocation = glGetUniformLocation(quadProgramID, "projection");
    //VERY IMPORTANT DO NOT DELETE THIS IT WILL DESTROY THE BLINN PHONG I SWEAR
    //I removed it haha

    glGenFramebuffers(1, &finalFBO);

    //create a texture for SSR
    glGenTextures(1, &finalTexture);
    glBindTexture(GL_TEXTURE_2D, finalTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenTextures(1, &ARMWORLDtex);
    glBindTexture(GL_TEXTURE_2D, ARMWORLDtex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glUseProgram(blurProgramID);

    glGenTextures(1, &actualFinalTexture);
    glBindTexture(GL_TEXTURE_2D, actualFinalTexture);
    //manually set the amount of mipmap levels :D
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenTextures(1, &Reflectionmask);
    glBindTexture(GL_TEXTURE_2D, Reflectionmask);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenVertexArrays(1, &blurVAO);
    glGenBuffers(1, &blurVBO);
    glBindVertexArray(blurVAO);
    glBindBuffer(GL_ARRAY_BUFFER, blurVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

    unsigned int finalattachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };

    glBindFramebuffer(GL_FRAMEBUFFER, finalFBO);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, actualFinalTexture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, finalTexture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, ARMWORLDtex, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, Reflectionmask, 0);

    glDrawBuffers(4, finalattachments);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "SOOOO CLOSE BRO BUT THE FINAL FRAMEBUFFER ACTUALLY BROKE WHICH SUCKS. NOT COMPLETE ERROR" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //attachments for sending the normals and albedo to SSR shader

    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    CamPosID = glGetUniformLocation(programID, "campos");
    numLightsID = glGetUniformLocation(programID, "numLights");
    LightColorID = glGetUniformLocation(programID, "LightColor");
    LightPosID = glGetUniformLocation(programID, "LightPos");
    MatrixID = glGetUniformLocation(programID, "HOPE");
    reflectivenessID = glGetUniformLocation(programID, "reflectiveness");
    meshcolorID = glGetUniformLocation(programID, "meshcolor");
    ModelMatrixID = glGetUniformLocation(programID, "Model");
    ViewMatrixID = glGetUniformLocation(programID, "View");
    SpecularID = glGetUniformLocation(programID, "spec");
    AmbientLightID = glGetUniformLocation(programID, "ambientlight");
    ggraphicsID = glGetUniformLocation(programID, "graphics");
    invprojectionv = glGetUniformLocation(quadProgramID, "invProjection");
    projectionv = glGetUniformLocation(quadProgramID, "projection");
    renderedTexturev = glGetUniformLocation(quadProgramID, "renderedTexture");
    depthTexturev = glGetUniformLocation(quadProgramID, "depthTexture");
    gNormalv = glGetUniformLocation(quadProgramID, "gNormal");
    gARMv = glGetUniformLocation(quadProgramID, "gARM");
    SSRgraphicsID = glGetUniformLocation(quadProgramID, "graphics");
    grainytexturev = glGetUniformLocation(blurProgramID, "grainytexture");
    gARMBlurv = glGetUniformLocation(blurProgramID, "gARM");
    reflectionmaskv = glGetUniformLocation(blurProgramID, "reflectionmask");
    blurgraphicsID = glGetUniformLocation(blurProgramID, "graphics");
    blurloc = glGetUniformLocation(blurProgramID, "weights");
    bluramount = glGetUniformLocation(blurProgramID, "size");
    shadowperspectiveID = glGetUniformLocation(programID, "DepthBiasMVP");
    shadowcubemapID = glGetUniformLocation(programID, "shadowmap");

    calculategaussian(blurstrengthb, blurminx);
}
/*
void CreateObjectWithAssimp() {} //coming soon!
*/

void CreateObjectWithOBJ(Object& obj) {
    glGenVertexArrays(1, &obj.VAO);
    glBindVertexArray(obj.VAO);
    glGenBuffers(1, &obj.VBO);

    glBindBuffer(GL_ARRAY_BUFFER, obj.VBO);
    glBufferData(GL_ARRAY_BUFFER, obj.vertices.size() * sizeof(glm::vec3), &obj.vertices[0], GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &obj.colorBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, obj.colorBuffer);
    glBufferData(GL_ARRAY_BUFFER, obj.uvs.size() * sizeof(glm::vec2), &obj.uvs[0], GL_DYNAMIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &obj.normalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, obj.normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, obj.normals.size() * sizeof(glm::vec3), &obj.normals[0], GL_DYNAMIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(2);

    glGenBuffers(1, &obj.tangentbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, obj.tangentbuffer);
    glBufferData(GL_ARRAY_BUFFER, obj.objtangents.size() * sizeof(glm::vec3), &obj.objtangents[0], GL_DYNAMIC_DRAW);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(3);

    glGenBuffers(1, &obj.EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, obj.indices.size() * sizeof(unsigned int), &obj.indices[0], GL_STATIC_DRAW);
    glBindVertexArray(0);
}

void Applysettings() {
    glUseProgram(programID);
    glUniform1i(ggraphicsID, GLint(graphicsquality));
    glUseProgram(quadProgramID);
    glUniform1i(SSRgraphicsID, GLint(graphicsquality));
    glUseProgram(blurProgramID);
    glUniform1i(blurgraphicsID, GLint(graphicsquality));
}

void InitUniforms() {
    glUseProgram(programID);
    std::cout << "HOPE: " << MatrixID << std::endl;
    std::cout << "Model: " << ModelMatrixID << std::endl;
    std::cout << "View Matrix" << ViewMatrixID << std::endl;

    std::cout << "AmbientLightID: " << AmbientLightID << std::endl;
    glUniform3f(AmbientLightID, ambientlight.x, ambientlight.y, ambientlight.z);

    std::cout << "SpecularID: " << SpecularID << std::endl;
    glUniform3f(SpecularID, spec.x, spec.y, spec.z);

    std::cout << "reflectiveness ID: " << reflectivenessID << std::endl;
    glUniform1f(reflectivenessID, reflectiveness);

    std::cout << "meshcolorID: " << meshcolorID << std::endl;
    glUniform3fv(meshcolorID, 1, &meshcolor[0]);

    std::cout << "LightPosID: " << LightPosID << std::endl;

    std::cout << "LightColorID: " << LightColorID << std::endl;

    std::cout << "numLightsID: " << numLightsID << std::endl;

    //Black Background
    glClearColor(0.0, 0.0, 0.0, 1.0);
    GLuint blockIndex = glGetUniformBlockIndex(programID, "LightBlock");
    glUniformBlockBinding(programID, blockIndex, 0);
}

void Loadmodel(GLuint width, GLuint height, Camera& cam) {
    HighProjection = glm::perspective(glm::radians(cam.FoV), (float) width / (float)height, 0.1f, 100.0f);
    invProj = glm::inverse(HighProjection);

    //needs to update every frame
    glUniform3f(CamPosID, cam.position.x, cam.position.y, cam.position.z);

    if (loopnumber == 1) {
        std::cout << "CamPosID: " << CamPosID << std::endl;
    }

    camView = cam.ViewMatrix;

    glm::mat4 hope = HighProjection;
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &hope[0][0]);

    glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &camView[0][0]);
}

int LoadTextures(const char* Texturelocation, bool isSRGB) {
    GLuint TextureID;
    if (stbi_info(Texturelocation, &TextureWidth, &TextureHeight, &TextureChannels)) {
        std::cout << "Texture valid! \n";
        if (TextureChannels == 3) {
            dataFormat = GL_RGB;
            internalFormat = isSRGB ? GL_SRGB : GL_RGB;
        } else if (TextureChannels == 4) {
            dataFormat = GL_RGBA;
            internalFormat = isSRGB ? GL_SRGB_ALPHA : GL_RGBA;
        }
        unsigned char* TextureData = stbi_load(Texturelocation, &TextureWidth, &TextureHeight, &TextureChannels, 0);
        if (TextureData != NULL) {
            glGenTextures(1, &TextureID);
            glBindTexture(GL_TEXTURE_2D, TextureID);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, TextureWidth, TextureHeight, 0, dataFormat, GL_UNSIGNED_BYTE, TextureData);
            glGenerateMipmap(GL_TEXTURE_2D);
            stbi_image_free(TextureData);
            return TextureID;
        } else {
            std::cout << "Super sorry bro but something brokie and the texture couldn't load. Bro WHO MADE THIS ENGINE?!?! \n";
            return 0;
        }
    } else {
        std::cout << "BRO that sucks really bro but sorry bro that image file isn't supported bro. Error: I lost count honestly so... Error your image didn't load I guess \n";
        return 0;
    }
}

void RenderPointLights() {
    ubostorage dumb;
    dumb.numLights = SceneLights.size();

    //Don't go over 500
    int count = std::min((int)SceneLights.size(), 500);

    for (int i = 0; i < count; i++) {
        dumb.LightPos[i] = glm::vec4(SceneLights[i].position, 1.0f);
        dumb.LightColor[i] = glm::vec4(SceneLights[i].color * SceneLights[i].brightness, 1.0f);
    }
    glBindBuffer(GL_UNIFORM_BUFFER, UBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(ubostorage), &dumb);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Modelparser() {
    Models.clear();
    for (Object& obj : Objects) {
        Model = glm::mat4(1.0f);
        //translate
        Model = glm::translate(Model, obj.position);

        //Rotate
        Model = glm::rotate(Model, glm::radians(obj.rotation.x), glm::vec3(1, 0, 0));
        Model = glm::rotate(Model, glm::radians(obj.rotation.y), glm::vec3(0, 1, 0));
        Model = glm::rotate(Model, glm::radians(obj.rotation.z), glm::vec3(0, 0, 1));
        //SCALE DO FIRST EVEN THOUGH IT LOOKS LAST I FREAKING SWEAR
        Model = glm::scale(Model, obj.scale);

        Models.push_back(Model);
    }
}

void RenderALL(GLuint width, GLuint height, Camera& cam) {
    glUseProgram(programID);
     Modelparser();
     //run shadows here
     //NOT DONE YET JUST TO SHOW A CONCEPT

    int mainloopnum = 0;
    if (loopnumber == 0) {
        InitUniforms();
    }
    RenderPointLights();
    Loadmodel(width, height, cam);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Main FBO BROKIE FATAL ERROR idk here's the error: " << std::endl;
    }

    for (Object& obj : Objects) {
        glm::mat4 currentmodel = Models[mainloopnum];

        glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &currentmodel[0][0]);

    //ooh color
        glm::vec3 finalColor = obj.color;
        glActiveTexture(GL_TEXTURE0);
        if (obj.selected == true) {
            finalColor = glm::mix(obj.color, glm::vec3(1.0f, 1.0f, 0.0f), 0.5f);
        }
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, (obj.diffuseMap != 0) ? obj.diffuseMap : albedoID);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, (obj.normalMap != 0) ? obj.normalMap : texturenormalID);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, (obj.armMap != 0) ? obj.armMap : ARMID);

        glUniform3fv(meshcolorID, 1, &finalColor[0]);

        glBindVertexArray(obj.VAO);
        if (obj.vertexCount > 0) {
            glBindBufferBase(GL_UNIFORM_BUFFER, 0, UBO);
            glDrawElements(GL_TRIANGLES, obj.vertexCount, GL_UNSIGNED_INT, 0);
        }
        mainloopnum++;
    }
    glBindVertexArray(0);
}

void UpdateBuffers(Object& obj) {
    if (obj.vertices.empty()) {
        std::cout << "Vertices Empty! \n";
        return;
    }


    // MUST BE FIRST I SWEAR
    glBindVertexArray(obj.VAO);

    // Positions
    glBindBuffer(GL_ARRAY_BUFFER, obj.VBO);
    glBufferData(GL_ARRAY_BUFFER, obj.vertices.size() * sizeof(glm::vec3), obj.vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    // UVs (Texture coords)
    glBindBuffer(GL_ARRAY_BUFFER, obj.colorBuffer);
    glBufferData(GL_ARRAY_BUFFER, obj.uvs.size() * sizeof(glm::vec2), obj.uvs.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(1);

    //Normals (Lighting)
    glBindBuffer(GL_ARRAY_BUFFER, obj.normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, obj.normals.size() * sizeof(glm::vec3), obj.normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, obj.tangentbuffer);
    glBufferData(GL_ARRAY_BUFFER, obj.objtangents.size() * sizeof(glm::vec3), obj.objtangents.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(3);

    std::cout << obj.name << " Tangent Buffer ID: " << obj.tangentbuffer << std::endl;
    std::cout << obj.name << " tangent count: " << obj.objtangents.size() << std::endl;

    if (!obj.objtangents.empty()) {
        glm::vec3 firstT = obj.objtangents[0];
        std::cout << obj.name << " first tangent: (" << firstT.x << ", " << firstT.y << ", " << firstT.z << ")" << std::endl;
    }

    if (obj.EBO == 0) {
        glGenBuffers(1, &obj.EBO);
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, obj.indices.size() * sizeof(unsigned int), obj.indices.data(), GL_STATIC_DRAW);

    //hola
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void calculategaussian(float blurstrength, int minx) {
    gaussian.clear();
    std::vector<float> weights;
    float sum = 0;
    int gaussianloop = 0;
    int maxx = -(minx);
    float gaussianfirstvar = 1.0f / (2 * 3.141592653589 *  glm::pow(blurstrength, 2));
    for (int x = minx; x < maxx + 1; x++) {
        for (int y = minx; y < maxx + 1; y++) {
            float gaussiansecondvar = -((glm::pow(x, 2) + glm::pow(y, 2)) / (2 * glm::pow(blurstrength, 2)));
            float e = glm::exp(gaussiansecondvar);
            weights.push_back(gaussianfirstvar * e);
            sum += gaussianfirstvar * e;
            gaussianloop++;
        }
    }
    for (int i = 0; i < gaussianloop; i++) {
        weights[i] = weights[i] / sum;
        gaussian.push_back(weights[i]);
    }
    blurminx = minx;
    blurstrengthb = blurstrength;

    glUniform1fv(blurloc, 81, &gaussian[0]);
    glUniform1i(bluramount, blurminx);
}

void OpenGLLoop(GLFWwindow* window, GLuint width, GLuint height, Camera& cam) {
    double EndTime = startTime;
    unsigned int attachments[3] = { GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
    unsigned int singlenumber = { GL_COLOR_ATTACHMENT0 };
    startTime = glfwGetTime();

    //Shadow Pass goes first! :D
    RenderALL(width, height, cam);

    glBindFramebuffer(GL_FRAMEBUFFER, finalFBO);
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDrawBuffers(3, attachments);

    glUseProgram(quadProgramID);

    glUniformMatrix4fv(invprojectionv, 1, GL_FALSE, glm::value_ptr(invProj));
    glUniformMatrix4fv(projectionv, 1, GL_FALSE, glm::value_ptr(HighProjection));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, renderedTexture);
    glUniform1i(renderedTexturev, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glGenerateMipmap(GL_TEXTURE_2D);
    glUniform1i(depthTexturev, 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glUniform1i(gNormalv, 2);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, gARM);
    glUniform1i(gARMv, 3);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, renderedTexture);

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glUseProgram(blurProgramID);

    glDrawBuffers(1, &singlenumber);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, finalTexture);
    glUniform1i(grainytexturev, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gARM);
    glUniform1i(gARMBlurv, 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, Reflectionmask);
    glUniform1i(reflectionmaskv, 2);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, finalTexture);

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "FRAMEBUFFER::INVALID\n";
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glUseProgram(0);
    glBindVertexArray(0);

    if (loopnumber < 2) {
        glm::mat4 v = cam.ViewMatrix;
        printf("Loop %d - View Matrix Row 0: %.2f, %.2f, %.2f, %.2f\n",
            loopnumber, v[0][0], v[1][0], v[2][0], v[3][0]);
        printf("Loop %d - View Matrix Row 3: %.2f, %.2f, %.2f, %.2f\n",
            loopnumber, v[0][3], v[1][3], v[2][3], v[3][3]);
    }
    loopnumber++;
    double deltatime = startTime - EndTime;
    FPS = std::round(1.0/deltatime);
}

/*
BELOW IS THE CODE FOR THE HIGHLY EXPERIMENTAL VULKAN VERSION OF HIGHLANDER CALLED HIGHLANDER AERIAL
*/

void createInstance() {

}

void InitVulkan(GLFWwindow* window, int width, int height) {

}

void Vulkancleanup() {

}

void VulkanLoop(GLFWwindow* window, int width, int height) {
    Vulkancleanup();
}
