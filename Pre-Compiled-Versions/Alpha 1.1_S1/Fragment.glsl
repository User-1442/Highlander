#version 420 core
#define MAX_LIGHTS 500
in vec2 UV;
in vec3 FragmentWorld; // actually fragment world position

in vec3 Normal;

in mat3 TBN;
in mat3 TBN_View;

layout (location = 0) out vec4 gAlbedo;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gARM;

layout (std140, binding = 0) uniform LightBlock {
    vec4 LightPos[500];
    vec4 LightColor[500];
    int numLights;
};

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D armMap;

uniform vec3 campos;
uniform vec3 ambientlight;
uniform vec3 spec;
uniform vec3 meshcolor;

uniform float reflectiveness;

void main() {

    vec3 normaltexture = texture(normalMap, UV).rgb;

    vec3 texturecolor = texture(albedoMap, UV).rgb;

    texturecolor = texturecolor * meshcolor;

    normaltexture = normaltexture * 2.0 - 1.0;

    vec3 NormalView = normalize(TBN_View * vec3(texture(normalMap, UV).rgb * 2.0 - 1.0)); //converts the flat texture onto the 3D stuff :D

    //initiallize and seperate ARM :DDDDDD (first integrated Alpha1.1)
    vec3 armData = texture(armMap, UV).rgb;

    float ao = armData.r;
    float roughness = armData.g;
    float metallic = armData.b;

    vec3 N = vec3(normalize(TBN * normaltexture));
    vec3 V = normalize(campos - FragmentWorld);

    vec3 NV = normalize(NormalView);

    vec3 totalDiffuse = vec3(0.0);
    vec3 specular = vec3(0.0);

    float specularpower = max(1.0, pow(reflectiveness, 1.0 - roughness));

    for (int i = 0; i < numLights; i++) {

        float dist = distance(LightPos[i].xyz, FragmentWorld);

        float attenuation = 1.0 / (dist * dist + 0.01);

        vec3 L = normalize(LightPos[i].xyz - FragmentWorld);
        vec3 H = normalize(L+V);

        float diffuseintensity = max(dot(N, L), 0.0);
        totalDiffuse += (texturecolor * LightColor[i].xyz * spec * diffuseintensity) * attenuation;

        float specularintensity = pow(max(0.0, dot(N, H)), specularpower);
        specular += (specularintensity * LightColor[i].xyz * spec) * attenuation;

    }

    //vec3 meshcolor = texture(myTextureSampler, UV).xyz;

    //ambient
    vec3 ambient = texturecolor * ambientlight * ao;
    totalDiffuse *= (1.0 - metallic);

    //diffuse

	//specular

	vec3 finalSpecular = mix(specular, specular * texturecolor, metallic);

	gAlbedo = vec4(ambient + totalDiffuse + finalSpecular, 1);

	gNormal = (NormalView + 1) / 2;

	gARM = vec3(ao, roughness, metallic);
}
