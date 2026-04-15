#version 420 core

uniform mat4 HOPE;
uniform mat4 Model;
uniform mat4 View;

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal_modelspace;
layout(location = 3) in vec3 vertexTangent_modelspace;

out vec2 UV;
out vec3 FragmentWorld;
out vec3 Normal;
out vec3 NormalView;

out mat3 TBN;
out mat3 TBN_View;

void main() {
    gl_Position = HOPE * View * Model * vec4(vertexPosition_modelspace, 1.0);

    vec3 T = normalize(vec3(Model * vec4(vertexTangent_modelspace, 0.0)));
    vec3 N = normalize(vec3(Model * vec4(vertexNormal_modelspace, 0.0)));

    T = normalize(T - dot(T, N) * N);

    vec3 B = cross(N, T);

    TBN = mat3(T, B, N); //crazy bro the TBN thingy is made

    T = normalize(vec3(View * Model * vec4(vertexTangent_modelspace, 0.0)));
    N = normalize(vec3(View * Model * vec4(vertexNormal_modelspace, 0.0)));

    T = normalize(T - dot(T, N) * N);

    B = cross(N, T);

    TBN_View = mat3(T, B, N);

    //actually fragment position
    FragmentWorld = (Model * vec4(vertexPosition_modelspace, 1.0)).xyz;

    NormalView = mat3(transpose(inverse(View * Model))) * vertexNormal_modelspace;

    Normal = mat3(transpose(inverse(Model))) * vertexNormal_modelspace;

    UV = vertexUV;
}
