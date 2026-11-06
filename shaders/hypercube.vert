#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 rot4D;
    mat4 model;
    mat4 view;
    mat4 proj;
    vec4 params; // x = viewerW, y = solidMode, z = time, w = unused
} ubo;

layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec4 inNormal;
layout(location = 2) in vec4 inColor;
layout(location = 3) in vec3 inTexCoord;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec3 fragTexCoord;
layout(location = 2) out vec4 fragNormal4D;
layout(location = 3) out float fragW;

void main() {
    // Rotate vertex and normal in 4D space
    vec4 pos4D = ubo.rot4D * inPosition;
    vec4 norm4D = ubo.rot4D * inNormal;

    // Perspective projection from 4D (XYZW) to 3D (XYZ)
    float viewerW = max(ubo.params.x, 1.8);
    float scale = viewerW / (viewerW - pos4D.w);
    vec3 pos3D = pos4D.xyz * scale;

    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(pos3D, 1.0);

    fragColor = inColor;
    fragTexCoord = inTexCoord;
    fragNormal4D = norm4D;
    fragW = pos4D.w;
}

