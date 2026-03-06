#version 420 core

uniform mat4 HOPE;
uniform mat4 Model;
uniform mat4 View;

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal_modelspace;

out vec2 UV;
out vec3 FragmentWorld;
out vec3 Normal;
out vec3 NormalView;

void main() {
    gl_Position = HOPE * View * Model * vec4(vertexPosition_modelspace, 1.0);

    //actually fragment position
    FragmentWorld = (Model * vec4(vertexPosition_modelspace, 1.0)).xyz;

    NormalView = mat3(transpose(inverse(View * Model))) * vertexNormal_modelspace;

    Normal = mat3(transpose(inverse(Model))) * vertexNormal_modelspace;

    UV = vertexUV;
}
