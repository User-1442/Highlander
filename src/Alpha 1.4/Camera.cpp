#define GLM_ENABLE_EXPERIMENTAL
#include "Camera.h"
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include <cmath>
#include "GUI.h"
#include "imgui/imgui.h"
#include "collisionman.h"
#include <iostream>

static bool iWasPressed = false;
static bool cursorhidden = true;
bool waspressed = false;
static bool firstMouse;
bool spacewaspressed = false;

int stupidwidth;
int stupidheight;

glm::vec3 walkup = glm::vec3(0, 1, 0);
glm::vec3 walkmask = glm::vec3(1.0f, 0.0f, 1.0f);

Camera::~Camera(){}

Camera::Camera(GLFWwindow* win) {
    window = win;
    position = glm::vec3(0, 0, 5); // Initialize the MEMBER variable
    horizontalAngle = 3.14f;
    verticalAngle = 0.0f;
    mouseSpeed = 0.005f;
    FoV = 45.0f;
}

glm::vec3 velocity = glm::vec3(0.0f);

void Camera::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    FoV -= float(yoffset); // scroll up = zoom in
    if (FoV < 1.0f) FoV = 1.0f;
    if (FoV > 90.0f) FoV = 90.0f;
}


void Camera::Update(GLFWwindow* window) {
    float speed = playerspeed;
    glfwGetWindowSize(window, &stupidwidth, &stupidheight);
    float centerX = stupidwidth / 2.0f;
    float centerY = stupidheight / 2.0f;
    static double lastTime = glfwGetTime();
    double currentTime = glfwGetTime();
    float deltaTime = float(currentTime - lastTime);
    lastTime = currentTime;
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    bool ispressed = glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS;
    bool spaceispressed = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;

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
        if (launchie == true) {
            if (glfwGetKey(window, GLFW_KEY_W ) == GLFW_PRESS){
                position += (direction * walkmask) * deltaTime * speed;
            }
// Move backward
            if (glfwGetKey(window, GLFW_KEY_S ) == GLFW_PRESS){
                position -= (direction * walkmask) * deltaTime * speed;
            }
// Strafe right
            if (glfwGetKey(window, GLFW_KEY_D ) == GLFW_PRESS){
                position += (right * walkmask) * deltaTime * speed;
            }
// Strafe left
            if (glfwGetKey(window, GLFW_KEY_A ) == GLFW_PRESS){
                position -= (right * walkmask) * deltaTime * speed;
            }
            velocity -= glm::vec3(0, 1, 0) * deltaTime * gravity;
            position += velocity * deltaTime;
            glm::vec3 oldposition = position;
            if (detectcollision(position, BVHtree) == true) {
                if (spaceispressed == true && spaceispressed != spacewaspressed) { //gosh this is a mess. So here's some comments. This controls jump
                    velocity.y += 5;
                }
            }
            glm::vec3 collisiondirection = position - oldposition;

            spacewaspressed = spaceispressed;
            //this resets the velocity if you hit the floor
            if (glm::dot(velocity, collisiondirection) < 0) {
                velocity = glm::vec3(0.0f);
            }
        } else {
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
                position += up * deltaTime * speed;
            }
            if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
                position -= up * deltaTime * speed;
            }
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
