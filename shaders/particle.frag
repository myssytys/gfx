#version 450

layout(location = 0) in vec4 inColor;
layout(location = 1) in vec2 inUV;

layout(location = 0) out vec4 outColor;

void main() {
    // Convert UV [0, 1] to centered coordinates [-1, 1]
    vec2 coord = inUV * 2.0 - 1.0;
    float distSq = dot(coord, coord);

    if (distSq > 1.0) {
        discard;
    }

    // Smooth exponential radial glow
    float alpha = exp(-distSq * 3.5);
    outColor = vec4(inColor.rgb, inColor.a * alpha);
}
