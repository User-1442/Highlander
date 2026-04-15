#version 420 core
//out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D renderedTexture;
uniform sampler2D depthTexture; //depth map :D
uniform sampler2D gNormal;
uniform sampler2D gARM;

uniform int graphics;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec3 gARMT;
layout (location = 2) out vec3 reflectionmask;

uniform mat4 projection; // osdkfjaoijdsofijasoiejfoisdjoifjaosiedjfoiajsdiofjo
uniform mat4 invProjection;

/*float hash(vec2 p) {
    return fract(sin(dot(p, vec2(12.9898, 78.233))) * 43758.5453);
}*/

void main() {
    float roughness = 0.0;
    float ao = 0.0;
    float metallic = 0.0;

    ao = texture(gARM, TexCoords).r;
    roughness = texture(gARM, TexCoords).g;
    metallic = texture(gARM, TexCoords).b;

    gARMT = texture(gARM, TexCoords).rgb;

    //float offset = hash(TexCoords);

    float depth = texture(depthTexture, TexCoords).r;
    float stepSize = 0.3;
    //vec4 ndcPos = vec4(TexCoords * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0); OLD CODE
    vec4 ndcPos = vec4(TexCoords * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
    vec4 viewPos = invProjection * ndcPos;
    viewPos /= viewPos.w;

    vec3 N = normalize(texture(gNormal, TexCoords).rgb) * 2.0 - 1.0;

    vec3 bias = N * 0.1;

    vec3 I = normalize(viewPos.xyz);

    vec3 R = normalize(reflect(I, N));//, vec3(hash(TexCoords + 0.1) * 2.0 - 1.0, hash(TexCoords + 0.2) * 2.0 - 1.0, hash(TexCoords + 0.3) * 2.0 - 1.0), 0.0f)); //ALSO WHEN REPAIRING IT PUT THE MIX BY ROUGHNESS BACK


    vec3 singlestep = R * stepSize;
    vec3 currentPos = viewPos.xyz + bias + (singlestep); //WHEN REPAIRING PUT * offset AFTER singlestep

    vec4 sceneColor = texture(renderedTexture, TexCoords);

    float linearZ = -viewPos.z; // Flip it because View Z is negative
    //FragColor = vec4(vec3(linearZ * 0.05), 1.0);

    FragColor = sceneColor;

    float thickness = 0.1;

    reflectionmask = vec3(0.0f);

    vec4 M_VP = vec4(0.0f);

    vec4 stepvec = projection * vec4(viewPos.xyz + (R * 100), 1.0);
    stepvec = vec4(stepvec.x / stepvec.w, stepvec.y / stepvec.w, stepvec.z / stepvec.w, 1.0);

    float newdepth = 0.0f;
    vec3 search = vec3(0.0f);
    vec3 End = stepvec.xyz * 0.5 + 0.5;
    vec4 newviewpos = projection * viewPos;
    newviewpos /= newviewpos.w;
    vec3 Beginning = newviewpos.xyz * 0.5 + 0.5;
    float inversestart = 1.0 / viewPos.w;
    float inversend = 1.0 / (projection * vec4(viewPos.xyz + (R * 100), 1.0)).w;
    float invstep = (inversend - inversestart) / 30.0f;
    newdepth = 0.0f;
    vec3 stepamount = (End - Beginning) / 30.0f;
    float viewFilter = smoothstep(0.0, 0.4, dot(-R, -I));
    float F0 = 0.04;
    float frensnel = F0 + (1.0 - F0) * pow(1.0 - max(dot(N, -I), 0.0), 5.0);
    float mask = frensnel * viewFilter;
    if (graphics < 3) {
        for (int i = 0; i < 30; i++) {
            float t = float(i) / 30.0;
            float inversew = mix(inversestart, inversend, t);
            float newt = (inversew - inversestart) / (inversend - inversestart);
            vec3 raypos = mix(Beginning, End, newt);
            newdepth = texture(depthTexture, raypos.xy).r;
            if (raypos.z > newdepth && raypos.z < newdepth + 0.02 && newdepth != 1.0 && mask != 0.0) {
                End = raypos.xyz;
                Beginning = raypos.xyz - stepamount;
                for (int j = 0; j < 5; j++) {
                    search = (Beginning + End) / 2.0;
                    newdepth = texture(depthTexture, search.xy).r;
                    if (search.z > newdepth) {
                        End = search.xyz;
                    } else {
                        Beginning = search.xyz;
                    }
                }
                vec3 newN = texture(gNormal, search.xy).rgb;
                newN = normalize(newN * 2.0 - 1.0);
                float normalFilter = smoothstep(0.6, 0.95, dot(newN, -R));
                mask = normalFilter;// * viewFilter;// * frensnel;
                vec3 reflections = texture(renderedTexture, search.xy).rgb;
                reflectionmask = vec3(mask * (reflections.r + reflections.g + reflections.b / 3.0));
                FragColor = vec4(mix(reflections, vec3(sceneColor), mask * (-roughness + 1)), 1.0);// WHEN REPAIRING ADD: (reflections.r + reflections.g + reflections.b) / 3) *
                break;
            }
        }
    }
}
