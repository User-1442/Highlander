#define GLM_ENABLE_EXPERIMENTAL
#include "Camera.h"
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include <cmath>
#include "imgui/imgui.h"

static bool iWasPressed = false;
static bool cursorhidden = true;
bool waspressed = false;
static bool firstMouse;

int stupidwidth;
int stupidheight;

Camera::~Camera(){}

Camera::Camera(GLFWwindow* win) {
    window = win;
    position = glm::vec3(0, 0, 5); // Initialize the MEMBER variable
    horizontalAngle = 3.14f;
    verticalAngle = 0.0f;
    speed = 3.0f;
    mouseSpeed = 0.005f;
    FoV = 45.0f;
}


void Camera::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    FoV -= float(yoffset); // scroll up = zoom in
    if (FoV < 1.0f) FoV = 1.0f;
    if (FoV > 90.0f) FoV = 90.0f;
}


void Camera::Update(GLFWwindow* window) {
    glfwGetWindowSize(window, &stupidwidth, &stupidheight);
    float centerX = stupidwidth / 2.0f;
    float centerY = stupidheight / 2.0f;
    static double lastTime = glfwGetTime();
    double currentTime = glfwGetTime();
    float deltaTime = float(currentTime - lastTime);
    lastTime = currentTime;
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    bool ispressed = glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS;

    if (cursorhidden == true) {
        glfwGetCursorPos(window, &xpos, &ypos);
        horizontalAngle += mouseSpeed * float(centerX - xpos);
        verticalAngle   += mouseSpeed * float(centerY - ypos);
        glfwSetCursorPos(window, centerX, centerY);
    }


    glm::vec3 direction(
        cos(verticalAngle) * sin(horizontalAngle),
        sin(verticalAngle),
        cos(verticalAngle) * cos(horizontalAngle)
    );

    glm::vec3 right = glm::vec3(
        sin(horizontalAngle - 3.14f/2.0f),
        0,
        cos(horizontalAngle - 3.14f/2.0f)
    );

    glm::vec3 up = glm::cross( right, direction );
    if (io.WantCaptureKeyboard == false) {
        if (glfwGetKey(window, GLFW_KEY_W ) == GLFW_PRESS){
            position += direction * deltaTime * speed;
        }
// Move backward
        if (glfwGetKey(window, GLFW_KEY_S ) == GLFW_PRESS){
            position -= direction * deltaTime * speed;
        }
// Strafe right
        if (glfwGetKey(window, GLFW_KEY_D ) == GLFW_PRESS){
            position += right * deltaTime * speed;
        }
// Strafe left
        if (glfwGetKey(window, GLFW_KEY_A ) == GLFW_PRESS){
            position -= right * deltaTime * speed;
        }
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
            ypos += 1;
        }
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
            ypos -= 1;
        }
    }
    if (ispressed == true && waspressed == false) {
        glfwSetCursorPos(window, centerX, centerY);
        cursorhidden = !cursorhidden;

        glfwSetInputMode(
            window,
            GLFW_CURSOR,
            cursorhidden ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL
        );

        firstMouse = true; // reset mouse delta
    }
    waspressed = ispressed;

    // Camera matrix
    ViewMatrix = glm::lookAt(
        position,           // Camera is here
        position+direction, // and looks here : at the same position, plus "direction"
        up                  // Head is up (set to 0,-1,0 to look upside-down)
    );
}
