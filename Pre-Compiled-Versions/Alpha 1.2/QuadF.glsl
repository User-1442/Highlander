#version 420 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D renderedTexture;
uniform sampler2D depthTexture; //depth map :D
uniform sampler2D gNormal;
uniform sampler2D gARM;

uniform mat4 projection; // osdkfjaoijdsofijasoiejfoisdjoifjaosiedjfoiajsdiofjo
uniform mat4 invProjection;

float hash(vec2 p) {
    return fract(sin(dot(p, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {
    float roughness = 0.0;
    float ao = 0.0;
    float metallic = 0.0;

    ao = texture(gARM, TexCoords).r;
    roughness = texture(gARM, TexCoords).g * 0.25;
    metallic = texture(gARM, TexCoords).b;

    float offset = hash(TexCoords);

    float depth = texture(depthTexture, TexCoords).r;
    float stepSize = 0.2;
    //vec4 ndcPos = vec4(TexCoords * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0); OLD CODE
    vec4 ndcPos = vec4(TexCoords * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
    vec4 viewPos = invProjection * ndcPos;
    viewPos /= viewPos.w;

    vec3 N = normalize(texture(gNormal, TexCoords).rgb) * 2.0 - 1.0;

    vec3 bias = N * 0.1;

    vec3 I = normalize(viewPos.xyz);

    vec3 R = normalize(mix(reflect(I, N), vec3(hash(TexCoords + 0.1) * 2.0 - 1.0, hash(TexCoords + 0.2) * 2.0 - 1.0, hash(TexCoords + 0.3) * 2.0 - 1.0), roughness));


    vec3 singlestep = R * stepSize;
    vec3 currentPos = viewPos.xyz + bias + (singlestep * offset);

    vec4 sceneColor = texture(renderedTexture, TexCoords);

    float linearZ = -viewPos.z; // Flip it because View Z is negative
    //FragColor = vec4(vec3(linearZ * 0.05), 1.0);

    FragColor = sceneColor;

    float thickness = 1.0;

    for (int i = 0; i < 100; i++) {
        if (i < 5) {
            currentPos += singlestep;
            continue;
        }
        vec3 A = currentPos - singlestep;
        vec4 projectedPos = projection * vec4(currentPos, 1.0);
        vec3 ndc = projectedPos.xyz / projectedPos.w;

        vec3 SampleCoords = (ndc + 1.0) / 2.0;

        if (SampleCoords.x >= 1.0 || SampleCoords.x <= 0.0 || SampleCoords.y >= 1.0 || SampleCoords.y <= 0.0) {
            break;
        }
        float sampledDepth = texture(depthTexture, SampleCoords.xy).r;
        vec4 NDCvar = vec4(SampleCoords.xy * 2.0 - 1.0, sampledDepth * 2.0 - 1.0, 1.0);
        vec4 actualViewPos = invProjection * NDCvar;
        actualViewPos /= actualViewPos.w;

        vec3 F0 = mix(vec3(0.04f, 0.04f, 0.04f), sceneColor.rgb, metallic);
        vec3 frensnel = F0 + (1.0 - F0) * pow(1.0 - max(dot(N, -I), 0.0), 5.0);

        if (currentPos.z < actualViewPos.z && currentPos.z > actualViewPos.z - thickness) {
            vec3 End = currentPos;
            vec3 Beginning = A;
            vec3 M = (Beginning + End) / 2.0;
            for (int j = 0; j < 5; j++) {
                vec4 M_ScreenSpace = projection * vec4(M, 1.0);
                vec3 M_ScreenSpacevec3 = M_ScreenSpace.xyz / M_ScreenSpace.w;
                vec3 M_SS = (M_ScreenSpacevec3 + 1.0) / 2.0;
                vec4 depthsample = texture(depthTexture, M_SS.xy);
                vec4 M_NDC = vec4(M_SS.xy * 2.0 - 1.0, depthsample.r * 2.0 - 1.0, 1.0);
                vec4 M_VP = invProjection * M_NDC;
                M_VP /= M_VP.w;
                if (M.z < M_VP.z) {
                    End = M;
                } else {
                    Beginning = M;
                }
                M = (Beginning + End) / 2.0;
            }
            vec4 Final_SS = projection * vec4(M, 1.0);
            vec2 finalCoords = (Final_SS.xy / Final_SS.w) * 0.5 + 0.5;

            vec3 newN = texture(gNormal, finalCoords).rgb;
            newN = normalize(newN * 2.0 - 1.0);

            float normalFilter = smoothstep(0.0, 0.2, dot(newN, -R));
            float viewFilter   = smoothstep(0.0, 0.2, dot(newN, -I));

            //float edgeFade = clamp(1.0 - max(abs(SampleCoords.x - 0.5), abs(SampleCoords.y - 0.5)) * 2.0, 0.0, 1.0);
            float mask = /*edgeFade * frensnel * */normalFilter * viewFilter;
            FragColor = vec4(mix(sceneColor.rgb, texture(renderedTexture, finalCoords).rgb, mask), 1.0);
            return;

        }
        currentPos += singlestep;
    }

}
