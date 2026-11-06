// HYPERICOSAHEDRON (600-CELL / HEXACOSICHORON / TETRAPLEX) CLASS
// CLOCKWISE WINDING ORDER
// 120 vertices on S^3 (where each vertex figure is a regular 3D icosahedron),
// 720 edges, 1200 triangular faces, and 600 regular tetrahedral 3-faces (cells):
//   - 2,400 vertices in Data (600 tetrahedral cells x 4 vertices per cell)
//   - 7,200 clockwise triangle indices (600 cells x 4 triangular faces x 3 indices)

#include "Hypericosa.h"
#include <algorithm>
#include <cmath>

namespace {

// Build the 120 unit 4D vertices of the 600-cell on S^3
std::vector<glm::vec4> build600CellUnitVertices() {
  std::vector<glm::vec4> verts;
  verts.reserve(120);

  const float phi = 1.61803398875f;
  const float invPhi = 0.61803398875f;

  // 1. 8 axial vertices: permutations of (+-1, 0, 0, 0)
  for (int axis = 0; axis < 4; ++axis) {
    for (int s : {1, -1}) {
      glm::vec4 v(0.0f);
      v[axis] = static_cast<float>(s);
      verts.push_back(v);
    }
  }

  // 2. 16 orthant vertices: (+-0.5, +-0.5, +-0.5, +-0.5)
  for (int mask = 0; mask < 16; ++mask) {
    verts.push_back(glm::vec4(
        (mask & 1) ? -0.5f : 0.5f,
        (mask & 2) ? -0.5f : 0.5f,
        (mask & 4) ? -0.5f : 0.5f,
        (mask & 8) ? -0.5f : 0.5f));
  }

  // 3. 96 golden vertices: even permutations of (+-phi/2, +-1/2, +-invPhi/2, 0)
  const float base[4] = {0.5f * phi, 0.5f, 0.5f * invPhi, 0.0f};
  const int evenPerms[12][4] = {
      {0, 1, 2, 3}, {0, 2, 3, 1}, {0, 3, 1, 2},
      {1, 0, 3, 2}, {1, 2, 0, 3}, {1, 3, 2, 0},
      {2, 0, 1, 3}, {2, 1, 3, 0}, {2, 3, 0, 1},
      {3, 0, 2, 1}, {3, 1, 0, 2}, {3, 2, 1, 0},
  };

  for (int p = 0; p < 12; ++p) {
    for (int signMask = 0; signMask < 8; ++signMask) {
      glm::vec4 v(0.0f);
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
        v[c] = val;
      }
      verts.push_back(v);
    }
  }

  return verts;
}

} // namespace

Hypericosa::Hypericosa(float size, uint32_t texDim) {
  this->size = size;

  const std::vector<glm::vec4> unitVerts = build600CellUnitVertices();
  const size_t nVerts = unitVerts.size(); // 120

  // Two unit vertices of the 600-cell share an edge iff dot(u_i, u_j) == phi / 2 (~0.809017)
  const float edgeDot = 0.809016994f;
  const float eps = 0.01f;

  // Build adjacency lists (only keeping neighbors j > i to enumerate 4-cliques once)
  std::vector<std::vector<size_t>> higherNeighbors(nVerts);
  for (size_t i = 0; i < nVerts; ++i) {
    for (size_t j = i + 1; j < nVerts; ++j) {
      if (std::abs(glm::dot(unitVerts[i], unitVerts[j]) - edgeDot) < eps) {
        higherNeighbors[i].push_back(j);
      }
    }
  }

  // Reference 3D tetrahedral texture coordinates in [0, 1]^3
  const glm::vec3 T0 = {0.5f, 1.0f, 0.5f}; // Apex
  const glm::vec3 T1 = {1.0f, 0.0f, 0.0f}; // Base-Right
  const glm::vec3 T2 = {0.0f, 0.0f, 0.0f}; // Base-Left
  const glm::vec3 T3 = {0.5f, 0.0f, 1.0f}; // Base-Front

  Data.reserve(600 * 4);
  indices.reserve(600 * 12);

  // Enumerate all 600 regular tetrahedral 3-faces (4-cliques)
  for (size_t i = 0; i < nVerts; ++i) {
    const auto &nbrs = higherNeighbors[i];
    for (size_t a = 0; a < nbrs.size(); ++a) {
      size_t j = nbrs[a];
      for (size_t b = a + 1; b < nbrs.size(); ++b) {
        size_t k = nbrs[b];
        if (std::abs(glm::dot(unitVerts[j], unitVerts[k]) - edgeDot) >= eps) {
          continue;
        }
        for (size_t c = b + 1; c < nbrs.size(); ++c) {
          size_t l = nbrs[c];
          if (std::abs(glm::dot(unitVerts[j], unitVerts[l]) - edgeDot) >= eps ||
              std::abs(glm::dot(unitVerts[k], unitVerts[l]) - edgeDot) >= eps) {
            continue;
          }

          // Found one of the 600 tetrahedral cells: (i, j, k, l)
          glm::vec4 v0 = unitVerts[i] * size;
          glm::vec4 v1 = unitVerts[j] * size;
          glm::vec4 v2 = unitVerts[k] * size;
          glm::vec4 v3 = unitVerts[l] * size;

          // Outward 4D unit normal is the normalized centroid of the 4 cell vertices
          glm::vec4 cellNormal =
              glm::normalize(unitVerts[i] + unitVerts[j] + unitVerts[k] + unitVerts[l]);

          // Ensure consistent positive 4D orientation det([v1 - v0, v2 - v0, v3 - v0, cellNormal]) > 0
          glm::mat4 orientMat(v1 - v0, v2 - v0, v3 - v0, cellNormal);
          if (glm::determinant(orientMat) < 0.0f) {
            std::swap(v2, v3);
          }

          auto makeVertexColor = [](const glm::vec4 &n) {
            float wTint = 0.85f + 0.15f * n.w;
            return glm::vec4(
                std::clamp((0.55f + 0.45f * n.x) * wTint, 0.15f, 1.0f),
                std::clamp((0.55f + 0.45f * n.y) * wTint, 0.15f, 1.0f),
                std::clamp((0.55f + 0.45f * n.z) * wTint, 0.15f, 1.0f),
                0.25f);
          };

          glm::vec4 n0 = glm::normalize(v0);
          glm::vec4 n1 = glm::normalize(v1);
          glm::vec4 n2 = glm::normalize(v2);
          glm::vec4 n3 = glm::normalize(v3);

          uint16_t baseIdx = static_cast<uint16_t>(Data.size());
          Data.push_back({v0, n0, makeVertexColor(n0), T0});
          Data.push_back({v1, n1, makeVertexColor(n1), T1});
          Data.push_back({v2, n2, makeVertexColor(n2), T2});
          Data.push_back({v3, n3, makeVertexColor(n3), T3});

          uint16_t cellIndices[12] = {
              // Right-front face (Apex, Base-Right, Base-Front)
              static_cast<uint16_t>(baseIdx + 0),
              static_cast<uint16_t>(baseIdx + 1),
              static_cast<uint16_t>(baseIdx + 3),
              // Left-front face (Apex, Base-Front, Base-Left)
              static_cast<uint16_t>(baseIdx + 0),
              static_cast<uint16_t>(baseIdx + 3),
              static_cast<uint16_t>(baseIdx + 2),
              // Back face (Apex, Base-Left, Base-Right)
              static_cast<uint16_t>(baseIdx + 0),
              static_cast<uint16_t>(baseIdx + 2),
              static_cast<uint16_t>(baseIdx + 1),
              // Bottom face (Base-Right, Base-Left, Base-Front)
              static_cast<uint16_t>(baseIdx + 1),
              static_cast<uint16_t>(baseIdx + 2),
              static_cast<uint16_t>(baseIdx + 3),
          };
          indices.insert(indices.end(), std::begin(cellIndices),
                         std::end(cellIndices));
        }
      }
    }
  }

  generateTexture3D(texDim);
}

void Hypericosa::generateTexture3D(uint32_t dim) {
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

glm::vec4 Hypericosa::sampleTexture3D(const glm::vec3 &uvw) const {
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

glm::vec3 Hypericosa::projectVertexTo3D(size_t vertexIndex,
                                        float viewerW) const {
  const glm::vec4 &p = Data.at(vertexIndex).pos;
  float denom = viewerW - p.w;
  if (std::abs(denom) < 1e-4f) {
    denom = (denom >= 0.0f) ? 1e-4f : -1e-4f;
  }
  float factor = viewerW / denom;
  return glm::vec3(p.x * factor, p.y * factor, p.z * factor);
}

Hypericosa::~Hypericosa() {}

