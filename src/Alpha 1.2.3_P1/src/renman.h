#include "winman.h"
#include "Camera.h"
#include "Scene.h"
#include <vector>

extern GLuint actualFinalTexture;
extern GLuint finalTexture;
extern double FPS;
void InitOpenGL(GLFWwindow* window, int width, int height);
void InitUniforms();
void OpenGLLoop(GLFWwindow* window, GLuint width, GLuint height, Camera& cam);
void Loadmodel(GLuint width, GLuint height, Camera& cam);
void UpdateBuffers(Object& obj);
void CreateObjectWithOBJ(Object& obj);
void Applysettings();

int LoadTextures(const char* Texturelocation, bool isSRGB);

extern float reflectiveness;
extern glm::vec3 ambientlight;
extern glm::vec3 spec;
extern int g_vertexCount;
extern char objfile[256];

extern std::vector<glm::vec3> tangents;
