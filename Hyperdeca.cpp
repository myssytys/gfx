// HYPERDODECAHEDRON (120-CELL / HECATONICOSACHORON) CLASS CLOCKWISE WINDING ORDER
// 120 dodecahedral 3-faces (cells), generated via the 120 unit quaternions of the
// binary icosahedral group 2I (vertices of the dual 600-cell on S^3).
// Each of the 120 dodecahedral cells has:
//   - 20 unique 4D vertices (2,400 vertices total in Data)
//   - 3D texture coordinates in [0, 1]^3 spanning the local dodecahedral cell
//   - 12 pentagonal 2-faces = 36 triangles = 108 clockwise indices per cell
//     (12,960 triangle indices total in indices).

#include "Hyperdeca.h"
#include <algorithm>
#include <array>
#include <cmath>

namespace {

// Multiply unit quaternion q = (qx, qy, qz, qw) (with scalar part qw)
// by 4D vector v = (vx, vy, vz, vw) (with scalar part vw):
// Maps (0, 0, 0, 1) -> q and rotates the tangent 3-space (x, y, z, 0)
// isometrically with det = +1 onto the tangent 3-space orthogonal to q.
glm::vec4 quatMul(const glm::vec4 &q, const glm::vec4 &v) {
  return glm::vec4(
      q.w * v.x + q.x * v.w + q.y * v.z - q.z * v.y,
      q.w * v.y - q.x * v.z + q.y * v.w + q.z * v.x,
      q.w * v.z + q.x * v.y - q.y * v.x + q.z * v.w,
      q.w * v.w - q.x * v.x - q.y * v.y - q.z * v.z);
}

// Build all 120 unit 4D cell normals (elements of the binary icosahedral group 2I)
std::vector<glm::vec4> build120CellNormals() {
  std::vector<glm::vec4> normals;
  normals.reserve(120);

  const float phi = 1.61803398875f;
  const float invPhi = 0.61803398875f;

  // 1. 8 axial unit vectors: permutations of (+-1, 0, 0, 0)
  for (int axis = 0; axis < 4; ++axis) {
    for (int s : {1, -1}) {
      glm::vec4 n(0.0f);
      n[axis] = static_cast<float>(s);
      normals.push_back(n);
    }
  }

  // 2. 16 orthant unit vectors: (+-0.5, +-0.5, +-0.5, +-0.5)
  for (int mask = 0; mask < 16; ++mask) {
    normals.push_back(glm::vec4(
        (mask & 1) ? -0.5f : 0.5f,
        (mask & 2) ? -0.5f : 0.5f,
        (mask & 4) ? -0.5f : 0.5f,
        (mask & 8) ? -0.5f : 0.5f));
  }

  // 3. 96 golden unit vectors: even permutations of (+-phi/2, +-1/2, +-invPhi/2, 0)
  const float base[4] = {0.5f * phi, 0.5f, 0.5f * invPhi, 0.0f};
  const int evenPerms[12][4] = {
      {0, 1, 2, 3}, {0, 2, 3, 1}, {0, 3, 1, 2},
      {1, 0, 3, 2}, {1, 2, 0, 3}, {1, 3, 2, 0},
      {2, 0, 1, 3}, {2, 1, 3, 0}, {2, 3, 0, 1},
      {3, 0, 2, 1}, {3, 1, 0, 2}, {3, 2, 1, 0},
  };

  for (int p = 0; p < 12; ++p) {
    for (int signMask = 0; signMask < 8; ++signMask) {
      glm::vec4 n(0.0f);
      int nonZeroBit = 0;
      for (int c = 0; c < 4; ++c) {
        int srcIdx = evenPerms[p][c];
        float val = base[srcIdx];
        if (srcIdx != 3) {
          if (signMask & (1 << nonZeroBit)) {
            val = -val;
          }
          nonZeroBit++;
        }
        n[c] = val;
      }
      normals.push_back(n);
    }
  }

  return normals;
}

} // namespace

Hyperdeca::Hyperdeca(float size, uint32_t texDim) {
  this->size = size;

  const float phi = 1.61803398875f;
  const float invPhi = 0.61803398875f;

  // Inradius R_in = size; local 3D dodecahedron scale r_cell = R_in * phi^-3
  // so that adjacent dodecahedral cells meet seamlessly across all 720 pentagonal faces.
  const float rIn = 1.0f * size;
  const float rCell = 0.2360679775f * size; // (sqrt(5) - 2) * size = phi^-3 * size

  const float a = 1.0f * rCell;
  const float b = invPhi * rCell;
  const float c = phi * rCell;

  // 20 local 3D vertices of a regular dodecahedron
  const std::array<glm::vec3, 20> localVerts = {{
      // 8 cube vertices (+-a, +-a, +-a)
      {a, a, a},    //  0
      {-a, a, a},   //  1
      {-a, -a, a},  //  2
      {a, -a, a},   //  3
      {a, a, -a},   //  4
      {-a, a, -a},  //  5
      {-a, -a, -a}, //  6
      {a, -a, -a},  //  7
      // 4 YZ golden rectangle vertices (0, +-c, +-b)
      {0.0f, c, b},   //  8
      {0.0f, c, -b},  //  9
      {0.0f, -c, b},  // 10
      {0.0f, -c, -b}, // 11
      // 4 XZ golden rectangle vertices (+-b, 0, +-c)
      {b, 0.0f, c},   // 12
      {-b, 0.0f, c},  // 13
      {b, 0.0f, -c},  // 14
      {-b, 0.0f, -c}, // 15
      // 4 XY golden rectangle vertices (+-c, +-b, 0)
      {c, b, 0.0f},   // 16
      {c, -b, 0.0f},  // 17
      {-c, b, 0.0f},  // 18
      {-c, -b, 0.0f}, // 19
  }};

  // 12 pentagonal faces of the local dodecahedron (clockwise vertex order matching Deca.cpp)
  const uint16_t localPentagons[12][5] = {
      {0, 12, 13, 1, 8},   // Face 0  (+Y, +Z)
      {0, 8, 9, 4, 16},    // Face 1  (+X, +Y)
      {0, 16, 17, 3, 12},  // Face 2  (+X, +Z)
      {4, 9, 5, 15, 14},   // Face 3  (+Y, -Z)
      {4, 14, 7, 17, 16},  // Face 4  (+X, -Z)
      {3, 10, 2, 13, 12},  // Face 5  (-Y, +Z)
      {3, 17, 7, 11, 10},  // Face 6  (+X, -Y)
      {7, 14, 15, 6, 11},  // Face 7  (-Y, -Z)
      {1, 18, 5, 9, 8},    // Face 8  (-X, +Y)
      {1, 13, 2, 19, 18},  // Face 9  (-X, +Z)
      {5, 18, 19, 6, 15},  // Face 10 (-X, -Z)
      {2, 10, 11, 6, 19},  // Face 11 (-X, -Y)
  };

  const std::vector<glm::vec4> cellNormals = build120CellNormals();

  Data.reserve(cellNormals.size() * 20);
  indices.reserve(cellNormals.size() * 12 * 9);

  for (size_t cellIdx = 0; cellIdx < cellNormals.size(); ++cellIdx) {
    const glm::vec4 &qNorm = cellNormals[cellIdx];

    // Smooth per-cell color derived from the 4D cell normal direction
    glm::vec4 cellColor(
        0.55f + 0.45f * qNorm.x,
        0.55f + 0.45f * qNorm.y,
        0.55f + 0.45f * qNorm.z,
        1.0f);
    // Modulate slightly by W hemisphere so Ana (+W) and Kata (-W) cells are distinguishable
    float wTint = 0.85f + 0.15f * qNorm.w;
    cellColor.r = std::clamp(cellColor.r * wTint, 0.15f, 1.0f);
    cellColor.g = std::clamp(cellColor.g * wTint, 0.15f, 1.0f);
    cellColor.b = std::clamp(cellColor.b * wTint, 0.15f, 1.0f);

    uint16_t baseVertex = static_cast<uint16_t>(Data.size());

    // Emit the 20 vertices of this dodecahedral cell
    for (size_t v = 0; v < 20; ++v) {
      const glm::vec3 &lv = localVerts[v];
      glm::vec4 canonicalPos(lv.x, lv.y, lv.z, rIn);
      glm::vec4 pos4D = quatMul(qNorm, canonicalPos);

      // Map local 3D dodecahedron coordinates [-c, +c]^3 into [0, 1]^3 texture space
      glm::vec3 uvw(
          (lv.x / (2.0f * c)) + 0.5f,
          (lv.y / (2.0f * c)) + 0.5f,
          (lv.z / (2.0f * c)) + 0.5f);

      Data.push_back({pos4D, qNorm, cellColor, uvw});
    }

    // Triangulate each of the 12 pentagonal faces (3 clockwise triangles per pentagon)
    for (int f = 0; f < 12; ++f) {
      uint16_t v0 = static_cast<uint16_t>(baseVertex + localPentagons[f][0]);
      uint16_t v1 = static_cast<uint16_t>(baseVertex + localPentagons[f][1]);
      uint16_t v2 = static_cast<uint16_t>(baseVertex + localPentagons[f][2]);
      uint16_t v3 = static_cast<uint16_t>(baseVertex + localPentagons[f][3]);
      uint16_t v4 = static_cast<uint16_t>(baseVertex + localPentagons[f][4]);

      indices.push_back(v0);
      indices.push_back(v1);
      indices.push_back(v2);

      indices.push_back(v0);
      indices.push_back(v2);
      indices.push_back(v3);

      indices.push_back(v0);
      indices.push_back(v3);
      indices.push_back(v4);
    }
  }

  generateTexture3D(texDim);
}

void Hyperdeca::generateTexture3D(uint32_t dim) {
  if (dim == 0) {
    dim = 16;
  }
  texWidth = dim;
  texHeight = dim;
  texDepth = dim;
  textureData3D.resize(static_cast<size_t>(texWidth) * texHeight * texDepth *
                       4);

  const float phi = 1.61803398875f;
  const float n1 = 0.5257311121f; // 1 / sqrt(1 + phi^2)
  const float n2 = 0.8506508084f; // phi / sqrt(1 + phi^2)
  // Distance from origin to each of the 12 dodecahedral face planes in normalized [-1, 1]^3 space
  const float faceDist = (n1 + n2) / phi;

  const glm::vec3 dodecaNormals[6] = {
      {0.0f, n1, n2},  {0.0f, -n1, n2},
      {n1, n2, 0.0f},  {-n1, n2, 0.0f},
      {n2, 0.0f, n1},  {n2, 0.0f, -n1},
  };

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

        // Convert [0, 1]^3 -> [-1, 1]^3 and measure proximity to the 12 dodecahedral face planes
        glm::vec3 p(2.0f * uNorm - 1.0f, 2.0f * vNorm - 1.0f,
                    2.0f * wNorm - 1.0f);
        int nearFaceCount = 0;
        for (int i = 0; i < 6; ++i) {
          float d = std::abs(glm::dot(p, dodecaNormals[i]));
          if (std::abs(d - faceDist) < 0.14f) {
            nearFaceCount++;
          }
        }

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

glm::vec4 Hyperdeca::sampleTexture3D(const glm::vec3 &uvw) const {
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

glm::vec3 Hyperdeca::projectVertexTo3D(size_t vertexIndex,
                                       float viewerW) const {
  const glm::vec4 &p = Data.at(vertexIndex).pos;
  float denom = viewerW - p.w;
  if (std::abs(denom) < 1e-4f) {
    denom = (denom >= 0.0f) ? 1e-4f : -1e-4f;
  }
  float factor = viewerW / denom;
  return glm::vec3(p.x * factor, p.y * factor, p.z * factor);
}

Hyperdeca::~Hyperdeca() {}

