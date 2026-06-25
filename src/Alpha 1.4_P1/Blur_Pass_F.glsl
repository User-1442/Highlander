#version 420 core

in vec2 TexCoords;

uniform sampler2D grainytexture;
uniform sampler2D gARM;
uniform sampler2D reflectionmask;

uniform int graphics;

uniform float weights[81];
uniform int size;

out vec4 FragColor;

void main() {
    ivec2 texturesize = textureSize(grainytexture, 0);
    vec2 texelsize = vec2(1.0 / texturesize.x, 1.0 / texturesize.y);
    float roughness = texture(gARM, TexCoords).g;
    float mask = (texture(reflectionmask, TexCoords).r + texture(reflectionmask, TexCoords).g + texture(reflectionmask, TexCoords).b) / 3;
    vec3 grainyinit = texture(grainytexture, TexCoords).rgb; //GRAINY INIT BRUV WOULD YOU LIKE A BOTOOL OF WATUH
    int minx = size;
    int maxx = -minx;
    int loopnumber = 0;

    vec3 result = vec3(0.0f);
    if (graphics < 2) {
        vec2 currentexel;
        for (int x = minx; x < maxx + 1; x++) {
            for (int y = minx; y < maxx + 1; y++) {
                currentexel = texelsize.xy * vec2(x, y);
                result += (texture(grainytexture, TexCoords + currentexel).rgb * weights[loopnumber]);
                loopnumber++;
            }
        }
        vec3 finalblurred = mix(grainyinit, result, mask);
        FragColor = vec4(finalblurred, 1.0);
    } else {
        FragColor = vec4(grainyinit, 1.0);
    }
    //ok so I multiplied the roughness by 2 and not 5 because multiplying it by 5 (the amount of mipmaps) made it a HOT MESS I MEAN GEEZ
}
