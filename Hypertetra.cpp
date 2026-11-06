// 4D TETRAHEDRON (5-CELL / PENTACHORON / 4-SIMPLEX) CLASS CLOCKWISE WINDING ORDER
// 5 tetrahedral 3-faces (cells), each with 4 vertices (20 vertices total)
// mapped to 3D tetrahedral texture coordinates in [0, 1]^3, and 4 triangular
// 2-faces per cell (12 indices per cell, 60 triangle indices total).

#include "Hypertetra.h"
#include <algorithm>
#include <cmath>

Hypertetra::Hypertetra(float edge, uint32_t texDim) {
  this->edge = edge;

  // Regular 4-simplex (5-cell) unit vertices on S^3 scaled by edge:
  //   V0 = ( +sqrt(5)/4, +sqrt(5)/4, +sqrt(5)/4, -1/4 ) * edge
  //   V1 = ( +sqrt(5)/4, -sqrt(5)/4, -sqrt(5)/4, -1/4 ) * edge
  //   V2 = ( -sqrt(5)/4, +sqrt(5)/4, -sqrt(5)/4, -1/4 ) * edge
  //   V3 = ( -sqrt(5)/4, -sqrt(5)/4, +sqrt(5)/4, -1/4 ) * edge
  //   V4 = (  0,          0,          0,         +1   ) * edge
  // Outward 4D unit normal for cell k (opposite vertex Vk) is N_k = -Vk / edge.

  const float c = 0.559016994f; // sqrt(5.0f) / 4.0f
  const float wBase = -0.25f;

  const glm::vec4 V0 = glm::vec4(c, c, c, wBase) * edge;
  const glm::vec4 V1 = glm::vec4(c, -c, -c, wBase) * edge;
  const glm::vec4 V2 = glm::vec4(-c, c, -c, wBase) * edge;
  const glm::vec4 V3 = glm::vec4(-c, -c, c, wBase) * edge;
  const glm::vec4 V4 = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f) * edge;

  // Outward 4D unit normals for each of the 5 tetrahedral cells
  const glm::vec4 N0 = glm::vec4(-c, -c, -c, 0.25f);
  const glm::vec4 N1 = glm::vec4(-c, c, c, 0.25f);
  const glm::vec4 N2 = glm::vec4(c, -c, c, 0.25f);
  const glm::vec4 N3 = glm::vec4(c, c, -c, 0.25f);
  const glm::vec4 N4 = glm::vec4(0.0f, 0.0f, 0.0f, -1.0f);

  // 3D texture coordinates for the 4 corners of each tetrahedral 3-face in [0, 1]^3
  const glm::vec3 T0 = {0.5f, 1.0f, 0.5f}; // Apex
  const glm::vec3 T1 = {1.0f, 0.0f, 0.0f}; // Base-Right
  const glm::vec3 T2 = {0.0f, 0.0f, 0.0f}; // Base-Left
  const glm::vec3 T3 = {0.5f, 0.0f, 1.0f}; // Base-Front

  // Distinct per-cell RGBA colors
  const glm::vec4 C0 = {1.0f, 0.25f, 0.25f, 1.0f}; // Red
  const glm::vec4 C1 = {0.25f, 1.0f, 0.35f, 1.0f}; // Green
  const glm::vec4 C2 = {0.25f, 0.55f, 1.0f, 1.0f}; // Blue
  const glm::vec4 C3 = {1.0f, 0.90f, 0.20f, 1.0f}; // Yellow
  const glm::vec4 C4 = {0.90f, 0.30f, 1.0f, 1.0f}; // Magenta

  Data = {
      // =====================================================================
      // CELL 0 (Opposite V0): Vertices (V1, V2, V3, V4), Normal = N0
      // =====================================================================
      {V1, N0, C0, T0}, // [0]
      {V2, N0, C0, T1}, // [1]
      {V3, N0, C0, T2}, // [2]
      {V4, N0, C0, T3}, // [3]

      // =====================================================================
      // CELL 1 (Opposite V1): Vertices (V0, V2, V4, V3), Normal = N1
      // =====================================================================
      {V0, N1, C1, T0}, // [4]
      {V2, N1, C1, T1}, // [5]
      {V4, N1, C1, T2}, // [6]
      {V3, N1, C1, T3}, // [7]

      // =====================================================================
      // CELL 2 (Opposite V2): Vertices (V0, V1, V3, V4), Normal = N2
      // =====================================================================
      {V0, N2, C2, T0}, // [8]
      {V1, N2, C2, T1}, // [9]
      {V3, N2, C2, T2}, // [10]
      {V4, N2, C2, T3}, // [11]

      // =====================================================================
      // CELL 3 (Opposite V3): Vertices (V0, V1, V4, V2), Normal = N3
      // =====================================================================
      {V0, N3, C3, T0}, // [12]
      {V1, N3, C3, T1}, // [13]
      {V4, N3, C3, T2}, // [14]
      {V2, N3, C3, T3}, // [15]

      // =====================================================================
      // CELL 4 (Base Cell Opposite V4): Vertices (V0, V1, V2, V3), Normal = N4
      // =====================================================================
      {V0, N4, C4, T0}, // [16]
      {V1, N4, C4, T1}, // [17]
      {V2, N4, C4, T2}, // [18]
      {V3, N4, C4, T3}, // [19]
  };

  // Build clockwise triangle indices for the 4 triangular faces of each of the 5 tetrahedral cells
  indices.reserve(5 * 12);
  for (uint16_t cell = 0; cell < 5; ++cell) {
    uint16_t b = cell * 4;
    uint16_t cellIndices[12] = {
        // Right-front face (Apex, Base-Right, Base-Front)
        static_cast<uint16_t>(b + 0),
        static_cast<uint16_t>(b + 1),
        static_cast<uint16_t>(b + 3),
        // Left-front face (Apex, Base-Front, Base-Left)
        static_cast<uint16_t>(b + 0),
        static_cast<uint16_t>(b + 3),
        static_cast<uint16_t>(b + 2),
        // Back face (Apex, Base-Left, Base-Right)
        static_cast<uint16_t>(b + 0),
        static_cast<uint16_t>(b + 2),
        static_cast<uint16_t>(b + 1),
        // Bottom face (Base-Right, Base-Left, Base-Front)
        static_cast<uint16_t>(b + 1),
        static_cast<uint16_t>(b + 2),
        static_cast<uint16_t>(b + 3),
    };
    indices.insert(indices.end(), std::begin(cellIndices),
                   std::end(cellIndices));
  }

  generateTexture3D(texDim);
}

void Hypertetra::generateTexture3D(uint32_t dim) {
  if (dim == 0) {
    dim = 16;
  }
  texWidth = dim;
  texHeight = dim;
  texDepth = dim;
  textureData3D.resize(static_cast<size_t>(texWidth) * texHeight * texDepth *
                       4);

  for (uint32_t z = 0; z < texDepth; ++z) {
    float wNorm = (texDepth > 1)
                      ? static_cast<float>(z) / static_cast<float>(texDepth - 1)
                      : 0.0f;

    for (uint32_t y = 0; y < texHeight; ++y) {
      float vNorm =
          (texHeight > 1)
              ? static_cast<float>(y) / static_cast<float>(texHeight - 1)
              : 0.0f;

      for (uint32_t x = 0; x < texWidth; ++x) {
        float uNorm =
            (texWidth > 1)
                ? static_cast<float>(x) / static_cast<float>(texWidth - 1)
                : 0.0f;

        size_t idx =
            ((static_cast<size_t>(z) * texHeight + y) * texWidth + x) * 4;

        // Barycentric coordinates with respect to the reference 3D tetrahedron:
        //   T0 = (0.5, 1.0, 0.5)
        //   T1 = (1.0, 0.0, 0.0)
        //   T2 = (0.0, 0.0, 0.0)
        //   T3 = (0.5, 0.0, 1.0)
        float l0 = vNorm;
        float l3 = wNorm - 0.5f * vNorm;
        float l1 = uNorm - 0.5f * vNorm - 0.5f * l3;
        float l2 = 1.0f - l0 - l1 - l3;

        // Count how many tetrahedral faces the voxel is close to (2+ = near a tetrahedral edge)
        int nearFaceCount = (std::abs(l0) < 0.12f ? 1 : 0) +
                            (std::abs(l1) < 0.12f ? 1 : 0) +
                            (std::abs(l2) < 0.12f ? 1 : 0) +
                            (std::abs(l3) < 0.12f ? 1 : 0);

        if (nearFaceCount >= 2) {
          textureData3D[idx + 0] = 255;
          textureData3D[idx + 1] = 255;
          textureData3D[idx + 2] = 255;
          textureData3D[idx + 3] = 255;
        } else {
          uint32_t checker = ((x / 2) + (y / 2) + (z / 2)) & 1u;
          float shade = checker ? 1.0f : 0.65f;

          uint8_t r = static_cast<uint8_t>(
              std::clamp((0.3f + 0.7f * uNorm) * shade * 255.0f, 0.0f, 255.0f));
          uint8_t g = static_cast<uint8_t>(
              std::clamp((0.3f + 0.7f * vNorm) * shade * 255.0f, 0.0f, 255.0f));
          uint8_t b = static_cast<uint8_t>(
              std::clamp((0.3f + 0.7f * wNorm) * shade * 255.0f, 0.0f, 255.0f));

          textureData3D[idx + 0] = r;
          textureData3D[idx + 1] = g;
          textureData3D[idx + 2] = b;
          textureData3D[idx + 3] = 255;
        }
      }
    }
  }
}

glm::vec4 Hypertetra::sampleTexture3D(const glm::vec3 &uvw) const {
  if (textureData3D.empty() || texWidth == 0 || texHeight == 0 ||
      texDepth == 0) {
    return glm::vec4(1.0f);
  }
  float u = std::clamp(uvw.x, 0.0f, 1.0f);
  float v = std::clamp(uvw.y, 0.0f, 1.0f);
  float w = std::clamp(uvw.z, 0.0f, 1.0f);

  uint32_t x = std::min(static_cast<uint32_t>(u * texWidth), texWidth - 1);
  uint32_t y = std::min(static_cast<uint32_t>(v * texHeight), texHeight - 1);
  uint32_t z = std::min(static_cast<uint32_t>(w * texDepth), texDepth - 1);

  size_t idx = ((static_cast<size_t>(z) * texHeight + y) * texWidth + x) * 4;
  return glm::vec4(
      textureData3D[idx + 0] / 255.0f, textureData3D[idx + 1] / 255.0f,
      textureData3D[idx + 2] / 255.0f, textureData3D[idx + 3] / 255.0f);
}

glm::vec3 Hypertetra::projectVertexTo3D(size_t vertexIndex,
                                        float viewerW) const {
  const glm::vec4 &p = Data.at(vertexIndex).pos;
  float denom = viewerW - p.w;
  if (std::abs(denom) < 1e-4f) {
    denom = (denom >= 0.0f) ? 1e-4f : -1e-4f;
  }
  float factor = viewerW / denom;
  return glm::vec3(p.x * factor, p.y * factor, p.z * factor);
}

Hypertetra::~Hypertetra() {}

