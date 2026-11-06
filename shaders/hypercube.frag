#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 rot4D;
    mat4 model;
    mat4 view;
    mat4 proj;
    vec4 params; // x = viewerW, y = solidMode, z = time, w = unused
} ubo;

layout(binding = 1) uniform sampler3D texSampler3D;

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec3 fragTexCoord;
layout(location = 2) in vec4 fragNormal4D;
layout(location = 3) in float fragW;

layout(location = 0) out vec4 outColor;

void main() {
    // Barycentric coordinates of the tetrahedral cell in [0, 1]^3 texture space:
    //   T0 = (0.5, 1.0, 0.5), T1 = (1.0, 0.0, 0.0), T2 = (0.0, 0.0, 0.0), T3 = (0.5, 0.0, 1.0)
    float l0 = fragTexCoord.y;
    float l3 = fragTexCoord.z - 0.5 * fragTexCoord.y;
    float l1 = fragTexCoord.x - 0.5 * fragTexCoord.y - 0.5 * l3;
    float l2 = 1.0 - l0 - l1 - l3;

    vec4 b = abs(vec4(l0, l1, l2, l3));

    // Find the two smallest barycentric distances:
    // bMin is ~0 on the triangular face itself; bSecondMin is the distance to the nearest triangle edge
    float bMin = min(min(b.x, b.y), min(b.z, b.w));
    float bSecondMin = 1.0;
    if (b.x == bMin) {
        bSecondMin = min(b.y, min(b.z, b.w));
    } else if (b.y == bMin) {
        bSecondMin = min(b.x, min(b.z, b.w));
    } else if (b.z == bMin) {
        bSecondMin = min(b.x, min(b.y, b.w));
    } else {
        bSecondMin = min(b.x, min(b.y, b.z));
    }

    // Sample the 3D texture using the 3D (u, v, w) texture coordinates
    vec4 tex3D = texture(texSampler3D, fragTexCoord);

    // 4D directional shading + W-depth cue
    vec4 lightDir4D = normalize(vec4(0.5, 0.7, 0.6, 0.8));
    float ndotl = 0.55 + 0.45 * abs(dot(normalize(fragNormal4D), lightDir4D));
    float wShade = clamp(0.75 + 0.25 * fragW, 0.45, 1.15);

    // Highlight tetrahedral cell edges and blend all faces/indices with alpha
    float edgeGlow = smoothstep(0.08, 0.0, bSecondMin);
    vec3 baseRgb = fragColor.rgb * (0.35 + 0.65 * tex3D.rgb) * ndotl * wShade;
    vec3 finalRgb = mix(baseRgb, vec3(1.0), edgeGlow * 0.65);

    float baseAlpha = (ubo.params.y > 0.5) ? 0.35 : 0.10;
    float alpha = mix(baseAlpha * fragColor.a * 4.0, 0.65, edgeGlow);

    outColor = vec4(finalRgb, clamp(alpha, 0.05, 0.85));
}
