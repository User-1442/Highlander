#version 420 core

layout(location = 0) in vec3 vertexPosition_modelspace;

uniform mat4 HOPE;

void main() {
	gl_Position = HOPE * vec4(vetexPosition_modelspace,1);
}