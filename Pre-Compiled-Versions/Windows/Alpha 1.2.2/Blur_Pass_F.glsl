#version 420 core

in vec2 TexCoords;

uniform sampler2D grainytexture;
uniform sampler2D gARM;
uniform sampler2D reflectionmask;

out vec4 FragColor;

void main() {
    ivec2 texturesize = textureSize(grainytexture, 0);
    vec2 texelsize = vec2(1 / texturesize.x, 1 / texturesize.y);
    float roughness = texture(gARM, TexCoords).g;
    float mask = (texture(reflectionmask, TexCoords).r + texture(reflectionmask, TexCoords).g + texture(reflectionmask, TexCoords).b) / 3;
    vec3 grainyinit = texture(grainytexture, TexCoords).rgb; //GRAINY INIT BRUV WOULD YOU LIKE A BOTOOL OF WATUH

    vec3 result = grainyinit;
    if (mask != 0.0) {
        for (int x = -1; x < 2; x++) {
            for (int y = -1; y < 2; y++) {
                vec2 currentexel = texelsize.xy * vec2(x, y);
                result += texture(grainytexture, TexCoords + currentexel).rgb;
            }
        }
        result /= 9;
    }
    vec3 finalblurred = mix(grainyinit, result, roughness);
    FragColor = vec4(finalblurred, 1.0);
    //ok so I multiplied the roughness by 2 and not 5 because multiplying it by 5 (the amount of mipmaps) made it a HOT MESS I MEAN GEEZ
}
