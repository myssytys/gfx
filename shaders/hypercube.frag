#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 rot4D;
    mat4 model;
    mat4 view;
    mat4 proj;
    vec4 params;   // x = viewerW, y = solidMode, z = time, w = viewerV
    vec4 angles5D; // x = angleXV, y = angleYV, z = angleZV, w = angleWV
} ubo;

layout(binding = 1) uniform sampler3D texSampler3D;

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec3 fragTexCoord;
layout(location = 2) in vec4 fragNormal4D;
layout(location = 3) in float fragW;

layout(location = 0) out vec4 outColor;

void main() {
    // Distance to the edges of each cubic 3-face in [0, 1]^3 texture space
    vec3 distToEdge = min(fragTexCoord, 1.0 - fragTexCoord);
    float dMin = min(distToEdge.x, min(distToEdge.y, distToEdge.z));
    float dMax = max(distToEdge.x, max(distToEdge.y, distToEdge.z));
    float dMid = distToEdge.x + distToEdge.y + distToEdge.z - dMin - dMax;

    // Sample the 3D procedural texture using (u, v, w) texture coordinates
    vec4 tex3D = texture(texSampler3D, fragTexCoord);

    // 4D directional shading + W-depth cue
    vec4 lightDir4D = normalize(vec4(0.5, 0.7, 0.6, 0.8));
    float ndotl = 0.55 + 0.45 * abs(dot(normalize(fragNormal4D), lightDir4D));
    float wShade = clamp(0.75 + 0.25 * fragW, 0.45, 1.15);

    // Highlight cubic cell edges and blend all 40 cubic 3-faces with 3D texture
    float edgeGlow = smoothstep(0.075, 0.015, dMid);
    vec3 baseRgb = mix(fragColor.rgb, tex3D.rgb, 0.65) * ndotl * wShade;
    vec3 finalRgb = mix(baseRgb, vec3(1.0), edgeGlow * 0.70);

    float faceAlpha = (ubo.params.y > 0.5) ? 0.32 : 0.11;
    float alpha = mix(faceAlpha * (0.6 + 0.4 * tex3D.a), 0.78, edgeGlow);

    outColor = vec4(finalRgb, clamp(alpha, 0.05, 0.88));
}
