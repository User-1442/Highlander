#version 420 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main()
{
    // the texture on the surface of the screen is 2D. So no z axis required :D
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);

    TexCoords = aTexCoords;
}
