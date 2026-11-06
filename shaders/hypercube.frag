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
    // Distance from the nearest [0, 1] boundary along each of the 3 texture axes
    vec3 distToEdge = min(fragTexCoord, 1.0 - fragTexCoord);

    // Sort distances: dMin is ~0 on the face itself, dMid is distance to nearest square edge
    float dMin = min(distToEdge.x, min(distToEdge.y, distToEdge.z));
    float dMax = max(distToEdge.x, max(distToEdge.y, distToEdge.z));
    float dMid = (distToEdge.x + distToEdge.y + distToEdge.z) - dMin - dMax;

    // In windowed mode (params.y < 0.5), cut out central window openings so inner 4D cells are visible
    if (ubo.params.y < 0.5) {
        bool onOuterFrame = (dMid < 0.14);
        bool onCrossRib   = (abs(fragTexCoord.x - 0.5) < 0.045 ||
                             abs(fragTexCoord.y - 0.5) < 0.045 ||
                             abs(fragTexCoord.z - 0.5) < 0.045);
        if (!onOuterFrame && !onCrossRib) {
            discard;
        }
    }

    // Sample the 3D texture using the 3D (u, v, w) texture coordinates
    vec4 tex3D = texture(texSampler3D, fragTexCoord);

    // 4D directional shading + W-depth cue
    vec4 lightDir4D = normalize(vec4(0.5, 0.7, 0.6, 0.8));
    float ndotl = 0.55 + 0.45 * abs(dot(normalize(fragNormal4D), lightDir4D));
    float wShade = clamp(0.75 + 0.25 * fragW, 0.45, 1.15);

    // Highlight cell edges
    float edgeGlow = smoothstep(0.06, 0.0, dMid);
    vec3 baseRgb = fragColor.rgb * (0.35 + 0.65 * tex3D.rgb) * ndotl * wShade;
    vec3 finalRgb = mix(baseRgb, vec3(1.0), edgeGlow * 0.65);

    outColor = vec4(finalRgb, 1.0);
}

