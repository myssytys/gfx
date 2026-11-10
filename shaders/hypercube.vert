#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 rot4D;
    mat4 model;
    mat4 view;
    mat4 proj;
    vec4 params;   // x = viewerW, y = solidMode, z = time, w = viewerV
    vec4 angles5D; // x = angleXV, y = angleYV, z = angleZV, w = angleWV
} ubo;

layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec4 inNormal;
layout(location = 2) in vec4 inColor;
layout(location = 3) in vec3 inTexCoord;
layout(location = 4) in vec2 inFifthDim; // x = v (5th coord), y = nv (5th normal)

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec3 fragTexCoord;
layout(location = 2) out vec4 fragNormal4D;
layout(location = 3) out float fragW;

void rotatePlane(inout float a, inout float b, float angle) {
    float c = cos(angle);
    float s = sin(angle);
    float na = a * c - b * s;
    float nb = a * s + b * c;
    a = na;
    b = nb;
}

void main() {
    // Full 5D vertex position (x, y, z, w, v) and 5D normal (nx, ny, nz, nw, nv)
    vec4 pos = inPosition;
    float v = inFifthDim.x;

    vec4 norm = inNormal;
    float nv = inFifthDim.y;

    // 1. Rotate in 5D space across XV, YV, ZV, and WV planes
    rotatePlane(pos.x, v, ubo.angles5D.x);
    rotatePlane(pos.y, v, ubo.angles5D.y);
    rotatePlane(pos.z, v, ubo.angles5D.z);
    rotatePlane(pos.w, v, ubo.angles5D.w);

    rotatePlane(norm.x, nv, ubo.angles5D.x);
    rotatePlane(norm.y, nv, ubo.angles5D.y);
    rotatePlane(norm.z, nv, ubo.angles5D.z);
    rotatePlane(norm.w, nv, ubo.angles5D.w);

    // 2. Perspective projection from 5D (XYZWV) to 4D (XYZW)
    float viewerV = max(ubo.params.w, 2.2);
    float scale5D = viewerV / (viewerV - v);
    vec4 pos4D = pos * scale5D;

    // 3. Rotate in 4D space (XW, YW, ZW, XY planes)
    pos4D = ubo.rot4D * pos4D;
    vec4 norm4D = ubo.rot4D * norm;

    // 4. Perspective projection from 4D (XYZW) to 3D (XYZ)
    float viewerW = max(ubo.params.x, 2.2);
    float scale4D = viewerW / (viewerW - pos4D.w);
    vec3 pos3D = pos4D.xyz * scale4D;

    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(pos3D, 1.0);

    fragColor = inColor;
    fragTexCoord = inTexCoord;
    fragNormal4D = norm4D;
    fragW = pos4D.w;
}
