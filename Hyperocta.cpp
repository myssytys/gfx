// HYPEROCTAHEDRON (16-CELL / HEXADECACHORON / 4D CROSS-POLYTOPE) CLASS
// CLOCKWISE WINDING ORDER
// 8 axial 4D vertices: (+-size, 0, 0, 0), (0, +-size, 0, 0), (0, 0, +-size, 0), (0, 0, 0, +-size)
// 16 tetrahedral 3-faces (cells), one per 4D orthant (+-X, +-Y, +-Z, +-W):
//   - 64 vertices total (16 cells x 4 vertices per tetrahedral cell)
//   - 192 clockwise triangle indices (16 cells x 4 triangular faces x 3 indices)

#include "Hyperocta.h"
#include <algorithm>
#include <cmath>

Hyperocta::Hyperocta(float size, uint32_t texDim) {
  this->size = size;
  const float s = 1.0f * size;
  const float norm = 0.5f; // 1.0f / sqrt(4.0f) in 4D

  // 8 axial vertices of the 4D cross-polytope
  const glm::vec4 XP = {s, 0.0f, 0.0f, 0.0f};
  const glm::vec4 XM = {-s, 0.0f, 0.0f, 0.0f};
  const glm::vec4 YP = {0.0f, s, 0.0f, 0.0f};
  const glm::vec4 YM = {0.0f, -s, 0.0f, 0.0f};
  const glm::vec4 ZP = {0.0f, 0.0f, s, 0.0f};
  const glm::vec4 ZM = {0.0f, 0.0f, -s, 0.0f};
  const glm::vec4 WP = {0.0f, 0.0f, 0.0f, s};
  const glm::vec4 WM = {0.0f, 0.0f, 0.0f, -s};

  // 3D tetrahedral texture coordinates in [0, 1]^3 for the 4 vertices of each cell
  const glm::vec3 T0 = {0.5f, 1.0f, 0.5f}; // 4D Apex (W-axis vertex)
  const glm::vec3 T1 = {1.0f, 0.0f, 0.0f}; // Base-Right
  const glm::vec3 T2 = {0.0f, 0.0f, 0.0f}; // Base-Left
  const glm::vec3 T3 = {0.5f, 0.0f, 1.0f}; // Base-Front

  Data = {
      // =====================================================================
      // --- ANA (+W) PYRAMID (8 Tetrahedral Cells with +W Apex) ---
      // =====================================================================

      // Cell 0: (+X, +Y, +Z, +W), parity = +1 -> (WP, XP, ZP, YP)
      {WP, {norm, norm, norm, norm}, {1.0f, 0.30f, 0.30f, 1.0f}, T0},   // [0]
      {XP, {norm, norm, norm, norm}, {1.0f, 0.30f, 0.30f, 1.0f}, T1},   // [1]
      {ZP, {norm, norm, norm, norm}, {1.0f, 0.30f, 0.30f, 1.0f}, T2},   // [2]
      {YP, {norm, norm, norm, norm}, {1.0f, 0.30f, 0.30f, 1.0f}, T3},   // [3]

      // Cell 1: (-X, +Y, +Z, +W), parity = -1 -> (WP, YP, ZP, XM)
      {WP, {-norm, norm, norm, norm}, {0.30f, 1.0f, 0.40f, 1.0f}, T0},  // [4]
      {YP, {-norm, norm, norm, norm}, {0.30f, 1.0f, 0.40f, 1.0f}, T1},  // [5]
      {ZP, {-norm, norm, norm, norm}, {0.30f, 1.0f, 0.40f, 1.0f}, T2},  // [6]
      {XM, {-norm, norm, norm, norm}, {0.30f, 1.0f, 0.40f, 1.0f}, T3},  // [7]

      // Cell 2: (-X, +Y, -Z, +W), parity = +1 -> (WP, XM, ZM, YP)
      {WP, {-norm, norm, -norm, norm}, {0.30f, 0.60f, 1.0f, 1.0f}, T0}, // [8]
      {XM, {-norm, norm, -norm, norm}, {0.30f, 0.60f, 1.0f, 1.0f}, T1}, // [9]
      {ZM, {-norm, norm, -norm, norm}, {0.30f, 0.60f, 1.0f, 1.0f}, T2}, // [10]
      {YP, {-norm, norm, -norm, norm}, {0.30f, 0.60f, 1.0f, 1.0f}, T3}, // [11]

      // Cell 3: (+X, +Y, -Z, +W), parity = -1 -> (WP, YP, ZM, XP)
      {WP, {norm, norm, -norm, norm}, {1.0f, 0.90f, 0.25f, 1.0f}, T0},  // [12]
      {YP, {norm, norm, -norm, norm}, {1.0f, 0.90f, 0.25f, 1.0f}, T1},  // [13]
      {ZM, {norm, norm, -norm, norm}, {1.0f, 0.90f, 0.25f, 1.0f}, T2},  // [14]
      {XP, {norm, norm, -norm, norm}, {1.0f, 0.90f, 0.25f, 1.0f}, T3},  // [15]

      // Cell 4: (+X, -Y, +Z, +W), parity = -1 -> (WP, YM, ZP, XP)
      {WP, {norm, -norm, norm, norm}, {1.0f, 0.35f, 0.95f, 1.0f}, T0},  // [16]
      {YM, {norm, -norm, norm, norm}, {1.0f, 0.35f, 0.95f, 1.0f}, T1},  // [17]
      {ZP, {norm, -norm, norm, norm}, {1.0f, 0.35f, 0.95f, 1.0f}, T2},  // [18]
      {XP, {norm, -norm, norm, norm}, {1.0f, 0.35f, 0.95f, 1.0f}, T3},  // [19]

      // Cell 5: (-X, -Y, +Z, +W), parity = +1 -> (WP, XM, ZP, YM)
      {WP, {-norm, -norm, norm, norm}, {0.25f, 0.95f, 0.95f, 1.0f}, T0}, // [20]
      {XM, {-norm, -norm, norm, norm}, {0.25f, 0.95f, 0.95f, 1.0f}, T1}, // [21]
      {ZP, {-norm, -norm, norm, norm}, {0.25f, 0.95f, 0.95f, 1.0f}, T2}, // [22]
      {YM, {-norm, -norm, norm, norm}, {0.25f, 0.95f, 0.95f, 1.0f}, T3}, // [23]

      // Cell 6: (-X, -Y, -Z, +W), parity = -1 -> (WP, YM, ZM, XM)
      {WP, {-norm, -norm, -norm, norm}, {1.0f, 0.60f, 0.20f, 1.0f}, T0}, // [24]
      {YM, {-norm, -norm, -norm, norm}, {1.0f, 0.60f, 0.20f, 1.0f}, T1}, // [25]
      {ZM, {-norm, -norm, -norm, norm}, {1.0f, 0.60f, 0.20f, 1.0f}, T2}, // [26]
      {XM, {-norm, -norm, -norm, norm}, {1.0f, 0.60f, 0.20f, 1.0f}, T3}, // [27]

      // Cell 7: (+X, -Y, -Z, +W), parity = +1 -> (WP, XP, ZM, YM)
      {WP, {norm, -norm, -norm, norm}, {0.70f, 0.40f, 1.0f, 1.0f}, T0},  // [28]
      {XP, {norm, -norm, -norm, norm}, {0.70f, 0.40f, 1.0f, 1.0f}, T1},  // [29]
      {ZM, {norm, -norm, -norm, norm}, {0.70f, 0.40f, 1.0f, 1.0f}, T2},  // [30]
      {YM, {norm, -norm, -norm, norm}, {0.70f, 0.40f, 1.0f, 1.0f}, T3},  // [31]

      // =====================================================================
      // --- KATA (-W) PYRAMID (8 Tetrahedral Cells with -W Apex) ---
      // =====================================================================

      // Cell 8: (+X, +Y, +Z, -W), parity = -1 -> (WM, YP, ZP, XP)
      {WM, {norm, norm, norm, -norm}, {0.95f, 0.45f, 0.45f, 1.0f}, T0},  // [32]
      {YP, {norm, norm, norm, -norm}, {0.95f, 0.45f, 0.45f, 1.0f}, T1},  // [33]
      {ZP, {norm, norm, norm, -norm}, {0.95f, 0.45f, 0.45f, 1.0f}, T2},  // [34]
      {XP, {norm, norm, norm, -norm}, {0.95f, 0.45f, 0.45f, 1.0f}, T3},  // [35]

      // Cell 9: (-X, +Y, +Z, -W), parity = +1 -> (WM, XM, ZP, YP)
      {WM, {-norm, norm, norm, -norm}, {0.45f, 0.95f, 0.55f, 1.0f}, T0}, // [36]
      {XM, {-norm, norm, norm, -norm}, {0.45f, 0.95f, 0.55f, 1.0f}, T1}, // [37]
      {ZP, {-norm, norm, norm, -norm}, {0.45f, 0.95f, 0.55f, 1.0f}, T2}, // [38]
      {YP, {-norm, norm, norm, -norm}, {0.45f, 0.95f, 0.55f, 1.0f}, T3}, // [39]

      // Cell 10: (-X, +Y, -Z, -W), parity = -1 -> (WM, YP, ZM, XM)
      {WM, {-norm, norm, -norm, -norm}, {0.45f, 0.70f, 0.95f, 1.0f}, T0}, // [40]
      {YP, {-norm, norm, -norm, -norm}, {0.45f, 0.70f, 0.95f, 1.0f}, T1}, // [41]
      {ZM, {-norm, norm, -norm, -norm}, {0.45f, 0.70f, 0.95f, 1.0f}, T2}, // [42]
      {XM, {-norm, norm, -norm, -norm}, {0.45f, 0.70f, 0.95f, 1.0f}, T3}, // [43]

      // Cell 11: (+X, +Y, -Z, -W), parity = +1 -> (WM, XP, ZM, YP)
      {WM, {norm, norm, -norm, -norm}, {0.95f, 0.85f, 0.40f, 1.0f}, T0}, // [44]
      {XP, {norm, norm, -norm, -norm}, {0.95f, 0.85f, 0.40f, 1.0f}, T1}, // [45]
      {ZM, {norm, norm, -norm, -norm}, {0.95f, 0.85f, 0.40f, 1.0f}, T2}, // [46]
      {YP, {norm, norm, -norm, -norm}, {0.95f, 0.85f, 0.40f, 1.0f}, T3}, // [47]

      // Cell 12: (+X, -Y, +Z, -W), parity = +1 -> (WM, XP, ZP, YM)
      {WM, {norm, -norm, norm, -norm}, {0.95f, 0.50f, 0.90f, 1.0f}, T0}, // [48]
      {XP, {norm, -norm, norm, -norm}, {0.95f, 0.50f, 0.90f, 1.0f}, T1}, // [49]
      {ZP, {norm, -norm, norm, -norm}, {0.95f, 0.50f, 0.90f, 1.0f}, T2}, // [50]
      {YM, {norm, -norm, norm, -norm}, {0.95f, 0.50f, 0.90f, 1.0f}, T3}, // [51]

      // Cell 13: (-X, -Y, +Z, -W), parity = -1 -> (WM, YM, ZP, XM)
      {WM, {-norm, -norm, norm, -norm}, {0.40f, 0.90f, 0.90f, 1.0f}, T0}, // [52]
      {YM, {-norm, -norm, norm, -norm}, {0.40f, 0.90f, 0.90f, 1.0f}, T1}, // [53]
      {ZP, {-norm, -norm, norm, -norm}, {0.40f, 0.90f, 0.90f, 1.0f}, T2}, // [54]
      {XM, {-norm, -norm, norm, -norm}, {0.40f, 0.90f, 0.90f, 1.0f}, T3}, // [55]

      // Cell 14: (-X, -Y, -Z, -W), parity = +1 -> (WM, XM, ZM, YM)
      {WM, {-norm, -norm, -norm, -norm}, {0.95f, 0.65f, 0.35f, 1.0f}, T0}, // [56]
      {XM, {-norm, -norm, -norm, -norm}, {0.95f, 0.65f, 0.35f, 1.0f}, T1}, // [57]
      {ZM, {-norm, -norm, -norm, -norm}, {0.95f, 0.65f, 0.35f, 1.0f}, T2}, // [58]
      {YM, {-norm, -norm, -norm, -norm}, {0.95f, 0.65f, 0.35f, 1.0f}, T3}, // [59]

      // Cell 15: (+X, -Y, -Z, -W), parity = -1 -> (WM, YM, ZM, XP)
      {WM, {norm, -norm, -norm, -norm}, {0.75f, 0.55f, 0.95f, 1.0f}, T0}, // [60]
      {YM, {norm, -norm, -norm, -norm}, {0.75f, 0.55f, 0.95f, 1.0f}, T1}, // [61]
      {ZM, {norm, -norm, -norm, -norm}, {0.75f, 0.55f, 0.95f, 1.0f}, T2}, // [62]
      {XP, {norm, -norm, -norm, -norm}, {0.75f, 0.55f, 0.95f, 1.0f}, T3}, // [63]
  };

  // Build clockwise triangle indices for the 4 triangular faces of each of the 16 tetrahedral cells
  indices.reserve(16 * 12);
  for (uint16_t cell = 0; cell < 16; ++cell) {
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

void Hyperocta::generateTexture3D(uint32_t dim) {
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

glm::vec4 Hyperocta::sampleTexture3D(const glm::vec3 &uvw) const {
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

glm::vec3 Hyperocta::projectVertexTo3D(size_t vertexIndex,
                                       float viewerW) const {
  const glm::vec4 &p = Data.at(vertexIndex).pos;
  float denom = viewerW - p.w;
  if (std::abs(denom) < 1e-4f) {
    denom = (denom >= 0.0f) ? 1e-4f : -1e-4f;
  }
  float factor = viewerW / denom;
  return glm::vec3(p.x * factor, p.y * factor, p.z * factor);
}

Hyperocta::~Hyperocta() {}

