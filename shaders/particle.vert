#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

struct Particle {
    vec4 pos;   // xyz = position, w = current life
    vec4 vel;   // xyz = velocity, w = max life
    vec4 color; // rgba
    vec4 scale; // x = size, y = rotation, z = rotSpeed, w = unused
};

layout(std430, binding = 1) readonly buffer ParticleBuffer {
    Particle particles[];
};

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec2 outUV;

// Triangle strip quad coordinates [-1, 1]
const vec2 QUAD_VERTICES[4] = vec2[](
    vec2(-1.0, -1.0),
    vec2( 1.0, -1.0),
    vec2(-1.0,  1.0),
    vec2( 1.0,  1.0)
);

const vec2 QUAD_UVS[4] = vec2[](
    vec2(0.0, 0.0),
    vec2(1.0, 0.0),
    vec2(0.0, 1.0),
    vec2(1.0, 1.0)
);

void main() {
    uint particleIndex = gl_InstanceIndex;
    uint cornerIndex = gl_VertexIndex;

    Particle p = particles[particleIndex];

    // Extract Camera Right and Up vectors directly from View Matrix for spherical billboarding
    vec3 camRight = vec3(ubo.view[0][0], ubo.view[1][0], ubo.view[2][0]);
    vec3 camUp    = vec3(ubo.view[0][1], ubo.view[1][1], ubo.view[2][1]);

    // 2D Rotation of quad corner
    vec2 corner = QUAD_VERTICES[cornerIndex] * p.scale.x;
    float cosAngle = cos(p.scale.y);
    float sinAngle = sin(p.scale.y);
    vec2 rotatedCorner = vec2(
        corner.x * cosAngle - corner.y * sinAngle,
        corner.x * sinAngle + corner.y * cosAngle
    );

    vec3 worldPos = p.pos.xyz + (camRight * rotatedCorner.x) + (camUp * rotatedCorner.y);

    gl_Position = ubo.proj * ubo.view * vec4(worldPos, 1.0);
    outColor = p.color;
    outUV = QUAD_UVS[cornerIndex];
}
