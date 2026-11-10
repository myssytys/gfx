// 5D HYPERCUBE (PENTERACT / 5-CUBE) CLASS CLOCKWISE WINDING ORDER
// 32 vertices in 5D: (+-size, +-size, +-size, +-size, +-size)
// 80 edges, 80 square 2-faces, 40 cubic 3-faces (cells), 10 tesseract 4-faces.
// Each of the 40 cubic 3-faces has:
//   - 8 vertices mapped to [0, 1]^3 3D texture coordinates (320 vertices total)
//   - 6 square 2-faces = 12 triangles = 36 clockwise indices per cubic cell
//     (1,440 triangle indices total).
// Projects from 5D (x, y, z, w, v) -> 4D (x, y, z, w) -> 3D (x, y, z).

#include "Penteract.h"
#include <algorithm>
#include <array>
#include <cmath>

Penteract::Penteract(float size, uint32_t texDim) {
  this->size = size;
  const float s = 1.0f * size;
  const float invSqrt2 = 0.70710678f;

  // 10 combinations of 3 free axes (a0, a1, a2) and 2 fixed axes (b0, b1) in 5D {0, 1, 2, 3, 4}
  struct CellAxes {
    int freeAxes[3];
    int fixedAxes[2];
  };

  const std::array<CellAxes, 10> axisCombinations = {{
      {{0, 1, 2}, {3, 4}}, // XYZ cube, fixed (W, V)
      {{0, 1, 3}, {2, 4}}, // XYW cube, fixed (Z, V)
      {{0, 2, 3}, {1, 4}}, // XZW cube, fixed (Y, V)
      {{1, 2, 3}, {0, 4}}, // YZW cube, fixed (X, V)
      {{0, 1, 4}, {2, 3}}, // XYV cube, fixed (Z, W)
      {{0, 2, 4}, {1, 3}}, // XZV cube, fixed (Y, W)
      {{1, 2, 4}, {0, 3}}, // YZV cube, fixed (X, W)
      {{0, 3, 4}, {1, 2}}, // XWV cube, fixed (Y, Z)
      {{1, 3, 4}, {0, 2}}, // YWV cube, fixed (X, Z)
      {{2, 3, 4}, {0, 1}}, // ZWV cube, fixed (X, Y)
  }};

  // Distinct color palette for the 10 axis-triple families
  const std::array<glm::vec3, 10> palette = {{
      {1.00f, 0.30f, 0.30f}, // Red
      {0.30f, 1.00f, 0.40f}, // Green
      {0.30f, 0.60f, 1.00f}, // Blue
      {1.00f, 0.90f, 0.25f}, // Yellow
      {1.00f, 0.35f, 0.95f}, // Magenta
      {0.25f, 0.95f, 0.95f}, // Cyan
      {1.00f, 0.60f, 0.20f}, // Orange
      {0.70f, 0.40f, 1.00f}, // Purple
      {0.45f, 1.00f, 0.75f}, // Mint
      {1.00f, 0.50f, 0.65f}, // Rose
  }};

  // Local 3D texture coordinates (u, v, w) for the 8 corners of a cubic 3-face
  const glm::vec3 localUVW[8] = {
      {0.0f, 0.0f, 0.0f}, // 0: (-1, -1, -1)
      {1.0f, 0.0f, 0.0f}, // 1: (+1, -1, -1)
      {1.0f, 1.0f, 0.0f}, // 2: (+1, +1, -1)
      {0.0f, 1.0f, 0.0f}, // 3: (-1, +1, -1)
      {0.0f, 0.0f, 1.0f}, // 4: (-1, -1, +1)
      {1.0f, 0.0f, 1.0f}, // 5: (+1, -1, +1)
      {1.0f, 1.0f, 1.0f}, // 6: (+1, +1, +1)
      {0.0f, 1.0f, 1.0f}, // 7: (-1, +1, +1)
  };

  Data.reserve(40 * 8);
  indices.reserve(40 * 36);

  for (size_t combIdx = 0; combIdx < axisCombinations.size(); ++combIdx) {
    const auto &axes = axisCombinations[combIdx];
    const glm::vec3 &baseRgb = palette[combIdx];

    // 4 sign combinations for the 2 fixed axes: (-1,-1), (+1,-1), (-1,+1), (+1,+1)
    for (int signMask = 0; signMask < 4; ++signMask) {
      float fixedSign0 = (signMask & 1) ? 1.0f : -1.0f;
      float fixedSign1 = (signMask & 2) ? 1.0f : -1.0f;

      // 5D outward unit normal along the 2 fixed axes
      float n5[5] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
      n5[axes.fixedAxes[0]] = fixedSign0 * invSqrt2;
      n5[axes.fixedAxes[1]] = fixedSign1 * invSqrt2;

      float shade = 0.85f + 0.075f * (fixedSign0 + fixedSign1);
      glm::vec4 cellColor(
          std::clamp(baseRgb.r * shade, 0.1f, 1.0f),
          std::clamp(baseRgb.g * shade, 0.1f, 1.0f),
          std::clamp(baseRgb.b * shade, 0.1f, 1.0f),
          0.25f);

      uint16_t baseVertex = static_cast<uint16_t>(Data.size());

      // Emit the 8 corner vertices of this cubic 3-face
      for (int corner = 0; corner < 8; ++corner) {
        const glm::vec3 &uvw = localUVW[corner];
        float p5[5] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f};

        p5[axes.freeAxes[0]] = (2.0f * uvw.x - 1.0f) * s;
        p5[axes.freeAxes[1]] = (2.0f * uvw.y - 1.0f) * s;
        p5[axes.freeAxes[2]] = (2.0f * uvw.z - 1.0f) * s;
        p5[axes.fixedAxes[0]] = fixedSign0 * s;
        p5[axes.fixedAxes[1]] = fixedSign1 * s;

        Vertex vert{};
        vert.pos = glm::vec4(p5[0], p5[1], p5[2], p5[3]);
        vert.normals = glm::vec4(n5[0], n5[1], n5[2], n5[3]);
        vert.colors = cellColor;
        vert.texCoords = uvw;
        vert.fifthDim = glm::vec2(p5[4], n5[4]);

        Data.push_back(vert);
      }

      // 6 square faces (12 clockwise triangles = 36 indices) of this cubic 3-face
      uint16_t b = baseVertex;
      uint16_t cellIndices[36] = {
          // Front face (+w_local, w_tex = 1)
          static_cast<uint16_t>(b + 5), static_cast<uint16_t>(b + 4), static_cast<uint16_t>(b + 7),
          static_cast<uint16_t>(b + 7), static_cast<uint16_t>(b + 6), static_cast<uint16_t>(b + 5),
          // Back face (-w_local, w_tex = 0)
          static_cast<uint16_t>(b + 0), static_cast<uint16_t>(b + 1), static_cast<uint16_t>(b + 2),
          static_cast<uint16_t>(b + 2), static_cast<uint16_t>(b + 3), static_cast<uint16_t>(b + 0),
          // Left face (-u_local, u_tex = 0)
          static_cast<uint16_t>(b + 7), static_cast<uint16_t>(b + 3), static_cast<uint16_t>(b + 0),
          static_cast<uint16_t>(b + 0), static_cast<uint16_t>(b + 4), static_cast<uint16_t>(b + 7),
          // Right face (+u_local, u_tex = 1)
          static_cast<uint16_t>(b + 1), static_cast<uint16_t>(b + 5), static_cast<uint16_t>(b + 6),
          static_cast<uint16_t>(b + 6), static_cast<uint16_t>(b + 2), static_cast<uint16_t>(b + 1),
          // Top face (+v_local, v_tex = 1)
          static_cast<uint16_t>(b + 6), static_cast<uint16_t>(b + 7), static_cast<uint16_t>(b + 3),
          static_cast<uint16_t>(b + 3), static_cast<uint16_t>(b + 2), static_cast<uint16_t>(b + 6),
          // Bottom face (-v_local, v_tex = 0)
          static_cast<uint16_t>(b + 1), static_cast<uint16_t>(b + 0), static_cast<uint16_t>(b + 4),
          static_cast<uint16_t>(b + 4), static_cast<uint16_t>(b + 5), static_cast<uint16_t>(b + 1),
      };
      indices.insert(indices.end(), std::begin(cellIndices),
                     std::end(cellIndices));
    }
  }

  projectAllTo3D();
  generateTexture3D(texDim);
}

void Penteract::generateTexture3D(uint32_t dim) {
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
    bool zBorder = (z == 0 || z == texDepth - 1);

    for (uint32_t y = 0; y < texHeight; ++y) {
      float vNorm =
          (texHeight > 1)
              ? static_cast<float>(y) / static_cast<float>(texHeight - 1)
              : 0.0f;
      bool yBorder = (y == 0 || y == texHeight - 1);

      for (uint32_t x = 0; x < texWidth; ++x) {
        float uNorm =
            (texWidth > 1)
                ? static_cast<float>(x) / static_cast<float>(texWidth - 1)
                : 0.0f;
        bool xBorder = (x == 0 || x == texWidth - 1);

        size_t idx =
            ((static_cast<size_t>(z) * texHeight + y) * texWidth + x) * 4;

        // Highlight 3D cubic cell edges where at least two coordinates lie on the boundary
        int borderCount =
            (xBorder ? 1 : 0) + (yBorder ? 1 : 0) + (zBorder ? 1 : 0);
        if (borderCount >= 2) {
          textureData3D[idx + 0] = 255;
          textureData3D[idx + 1] = 255;
          textureData3D[idx + 2] = 255;
          textureData3D[idx + 3] = 255;
        } else {
          uint32_t checker = ((x / 2) + (y / 2) + (z / 2)) & 1u;
          float shade = checker ? 1.0f : 0.65f;

          uint8_t r = static_cast<uint8_t>(std::clamp(
              (0.25f + 0.75f * uNorm) * shade * 255.0f, 0.0f, 255.0f));
          uint8_t g = static_cast<uint8_t>(std::clamp(
              (0.25f + 0.75f * vNorm) * shade * 255.0f, 0.0f, 255.0f));
          uint8_t b = static_cast<uint8_t>(std::clamp(
              (0.25f + 0.75f * wNorm) * shade * 255.0f, 0.0f, 255.0f));

          textureData3D[idx + 0] = r;
          textureData3D[idx + 1] = g;
          textureData3D[idx + 2] = b;
          textureData3D[idx + 3] = 255;
        }
      }
    }
  }
}

glm::vec4 Penteract::sampleTexture3D(const glm::vec3 &uvw) const {
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

glm::vec4 Penteract::projectVertexTo4D(size_t vertexIndex,
                                       float viewerV) const {
  const Vertex &vert = Data.at(vertexIndex);
  float v5 = vert.fifthDim.x;
  float denom5 = viewerV - v5;
  if (std::abs(denom5) < 1e-4f) {
    denom5 = (denom5 >= 0.0f) ? 1e-4f : -1e-4f;
  }
  float scale5 = viewerV / denom5;
  return vert.pos * scale5;
}

glm::vec3 Penteract::projectVertexTo3D(size_t vertexIndex, float viewerV,
                                       float viewerW) const {
  glm::vec4 pos4D = projectVertexTo4D(vertexIndex, viewerV);
  float denom4 = viewerW - pos4D.w;
  if (std::abs(denom4) < 1e-4f) {
    denom4 = (denom4 >= 0.0f) ? 1e-4f : -1e-4f;
  }
  float scale4 = viewerW / denom4;
  return glm::vec3(pos4D.x * scale4, pos4D.y * scale4, pos4D.z * scale4);
}

void Penteract::projectAllTo3D(float viewerV, float viewerW) {
  projected3D.resize(Data.size());
  for (size_t i = 0; i < Data.size(); ++i) {
    projected3D[i] = projectVertexTo3D(i, viewerV, viewerW);
  }
}

Penteract::~Penteract() {}

