#version 420 core
#define MAX_LIGHTS 500
in vec2 UV;
in vec3 FragmentWorld; // actually fragment world position

in vec3 Normal;
in vec3 NormalView;

layout (location = 0) out vec4 gAlbedo;
layout (location = 1) out vec3 gNormal;

layout (std140, binding = 0) uniform LightBlock {
    vec4 LightPos[500];
    vec4 LightColor[500];
    int numLights;
};

uniform sampler2D myTextureSampler;
uniform vec3 campos;
uniform vec3 ambientlight;
uniform vec3 spec;
uniform vec3 meshcolor;

uniform float reflectiveness;

void main() {
    vec3 N = normalize(Normal);
    vec3 V = normalize(campos - FragmentWorld);

    vec3 NV = normalize(NormalView);

    vec3 totalDiffuse = vec3(0.0);
    vec3 specular = vec3(0.0);

    for (int i = 0; i < numLights; i++) {

        float dist = distance(LightPos[i].xyz, FragmentWorld);

        float attenuation = 1.0 / (dist * dist + 0.01);

        vec3 L = normalize(LightPos[i].xyz - FragmentWorld);
        vec3 H = normalize(L+V);

        float diffuseintensity = max(dot(N, L), 0.0);
        totalDiffuse += (meshcolor * LightColor[i].xyz * spec) * attenuation;

        float specularintensity = pow(max(0.0, dot(N, H)), reflectiveness);
        specular += (specularintensity * LightColor[i].xyz * spec) * attenuation;

    }

    //vec3 meshcolor = texture(myTextureSampler, UV).xyz;

    //ambient
    vec3 ambient = meshcolor * ambientlight;

    //diffuse

	//specular

	gAlbedo = vec4(ambient + totalDiffuse + specular, 1);
	gNormal = NormalView;
}
