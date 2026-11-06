// HYPERCUBE (TESSERACT) CLASS CLOCKWISE WINDING ORDER
// 8 cubic 3-faces (cells): +W, -W, -X, +X, +Y, -Y, +Z, -Z
// Each cubic cell has 8 vertices mapped to [0, 1]^3 3D texture coordinates
// and 6 square 2-faces (12 triangles = 36 indices per cell, 288 indices total).

#include "Hypercube.h"
#include <algorithm>
#include <cmath>

Hypercube::Hypercube(float size, uint32_t texDim) {
  this->size = size;
  const float s = 1.0f * size;

  // Each of the 8 cubic cells has 8 vertices ordered by local 3D texCoords (u, v, w):
  //   local [0]: (0, 0, 0) -> (-s, -s, -s)
  //   local [1]: (1, 0, 0) -> (+s, -s, -s)
  //   local [2]: (1, 1, 0) -> (+s, +s, -s)
  //   local [3]: (0, 1, 0) -> (-s, +s, -s)
  //   local [4]: (0, 0, 1) -> (-s, -s, +s)
  //   local [5]: (1, 0, 1) -> (+s, -s, +s)
  //   local [6]: (1, 1, 1) -> (+s, +s, +s)
  //   local [7]: (0, 1, 1) -> (-s, +s, +s)
  // Local axes (e_u, e_v, e_w) are chosen so det([e_u, e_v, e_w, normal4D]) = +1.

  Data = {
      // =====================================================================
      // CELL 0: Ana / Front-4D Cell (+W, w = +s), Normal = (0, 0, 0, +1)
      // Local axes: (u, v, w) -> (+X, +Y, +Z)
      // =====================================================================
      // [0] (0, 0, 0)
      {{-s, -s, -s, s}, {0.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},
      // [1] (1, 0, 0)
      {{s, -s, -s, s}, {0.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
      // [2] (1, 1, 0)
      {{s, s, -s, s}, {0.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 0.0f}},
      // [3] (0, 1, 0)
      {{-s, s, -s, s}, {0.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
      // [4] (0, 0, 1)
      {{-s, -s, s, s}, {0.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
      // [5] (1, 0, 1)
      {{s, -s, s, s}, {0.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 1.0f}},
      // [6] (1, 1, 1)
      {{s, s, s, s}, {0.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}},
      // [7] (0, 1, 1)
      {{-s, s, s, s}, {0.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 1.0f}},

      // =====================================================================
      // CELL 1: Kata / Back-4D Cell (-W, w = -s), Normal = (0, 0, 0, -1)
      // Local axes: (u, v, w) -> (-X, +Y, +Z)
      // =====================================================================
      // [8] (0, 0, 0)
      {{s, -s, -s, -s}, {0.0f, 0.0f, 0.0f, -1.0f}, {1.0f, 1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},
      // [9] (1, 0, 0)
      {{-s, -s, -s, -s}, {0.0f, 0.0f, 0.0f, -1.0f}, {1.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
      // [10] (1, 1, 0)
      {{-s, s, -s, -s}, {0.0f, 0.0f, 0.0f, -1.0f}, {1.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 0.0f}},
      // [11] (0, 1, 0)
      {{s, s, -s, -s}, {0.0f, 0.0f, 0.0f, -1.0f}, {1.0f, 1.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
      // [12] (0, 0, 1)
      {{s, -s, s, -s}, {0.0f, 0.0f, 0.0f, -1.0f}, {1.0f, 1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
      // [13] (1, 0, 1)
      {{-s, -s, s, -s}, {0.0f, 0.0f, 0.0f, -1.0f}, {1.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 1.0f}},
      // [14] (1, 1, 1)
      {{-s, s, s, -s}, {0.0f, 0.0f, 0.0f, -1.0f}, {1.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}},
      // [15] (0, 1, 1)
      {{s, s, s, -s}, {0.0f, 0.0f, 0.0f, -1.0f}, {1.0f, 1.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 1.0f}},

      // =====================================================================
      // CELL 2: Left Cell (-X, x = -s), Normal = (-1, 0, 0, 0)
      // Local axes: (u, v, w) -> (+Y, +Z, +W)
      // =====================================================================
      // [16] (0, 0, 0)
      {{-s, -s, -s, -s}, {-1.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},
      // [17] (1, 0, 0)
      {{-s, s, -s, -s}, {-1.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
      // [18] (1, 1, 0)
      {{-s, s, s, -s}, {-1.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 0.0f}},
      // [19] (0, 1, 0)
      {{-s, -s, s, -s}, {-1.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
      // [20] (0, 0, 1)
      {{-s, -s, -s, s}, {-1.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
      // [21] (1, 0, 1)
      {{-s, s, -s, s}, {-1.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 1.0f}},
      // [22] (1, 1, 1)
      {{-s, s, s, s}, {-1.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}},
      // [23] (0, 1, 1)
      {{-s, -s, s, s}, {-1.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 1.0f}},

      // =====================================================================
      // CELL 3: Right Cell (+X, x = +s), Normal = (+1, 0, 0, 0)
      // Local axes: (u, v, w) -> (-Y, +Z, +W)
      // =====================================================================
      // [24] (0, 0, 0)
      {{s, s, -s, -s}, {1.0f, 0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},
      // [25] (1, 0, 0)
      {{s, -s, -s, -s}, {1.0f, 0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
      // [26] (1, 1, 0)
      {{s, -s, s, -s}, {1.0f, 0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 0.0f}},
      // [27] (0, 1, 0)
      {{s, s, s, -s}, {1.0f, 0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
      // [28] (0, 0, 1)
      {{s, s, -s, s}, {1.0f, 0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
      // [29] (1, 0, 1)
      {{s, -s, -s, s}, {1.0f, 0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 1.0f}},
      // [30] (1, 1, 1)
      {{s, -s, s, s}, {1.0f, 0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}},
      // [31] (0, 1, 1)
      {{s, s, s, s}, {1.0f, 0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 1.0f}},

      // =====================================================================
      // CELL 4: Top Cell (+Y, y = +s), Normal = (0, +1, 0, 0)
      // Local axes: (u, v, w) -> (+X, +Z, -W)
      // =====================================================================
      // [32] (0, 0, 0)
      {{-s, s, -s, s}, {0.0f, 1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},
      // [33] (1, 0, 0)
      {{s, s, -s, s}, {0.0f, 1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
      // [34] (1, 1, 0)
      {{s, s, s, s}, {0.0f, 1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 0.0f}},
      // [35] (0, 1, 0)
      {{-s, s, s, s}, {0.0f, 1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
      // [36] (0, 0, 1)
      {{-s, s, -s, -s}, {0.0f, 1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
      // [37] (1, 0, 1)
      {{s, s, -s, -s}, {0.0f, 1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 1.0f}},
      // [38] (1, 1, 1)
      {{s, s, s, -s}, {0.0f, 1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}},
      // [39] (0, 1, 1)
      {{-s, s, s, -s}, {0.0f, 1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 1.0f}},

      // =====================================================================
      // CELL 5: Bottom Cell (-Y, y = -s), Normal = (0, -1, 0, 0)
      // Local axes: (u, v, w) -> (+X, +Z, +W)
      // =====================================================================
      // [40] (0, 0, 0)
      {{-s, -s, -s, -s}, {0.0f, -1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},
      // [41] (1, 0, 0)
      {{s, -s, -s, -s}, {0.0f, -1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
      // [42] (1, 1, 0)
      {{s, -s, s, -s}, {0.0f, -1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 0.0f}},
      // [43] (0, 1, 0)
      {{-s, -s, s, -s}, {0.0f, -1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
      // [44] (0, 0, 1)
      {{-s, -s, -s, s}, {0.0f, -1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
      // [45] (1, 0, 1)
      {{s, -s, -s, s}, {0.0f, -1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 1.0f}},
      // [46] (1, 1, 1)
      {{s, -s, s, s}, {0.0f, -1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}},
      // [47] (0, 1, 1)
      {{-s, -s, s, s}, {0.0f, -1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 1.0f}},

      // =====================================================================
      // CELL 6: Front Cell (+Z, z = +s), Normal = (0, 0, +1, 0)
      // Local axes: (u, v, w) -> (+X, +Y, -W)
      // =====================================================================
      // [48] (0, 0, 0)
      {{-s, -s, s, s}, {0.0f, 0.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},
      // [49] (1, 0, 0)
      {{s, -s, s, s}, {0.0f, 0.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
      // [50] (1, 1, 0)
      {{s, s, s, s}, {0.0f, 0.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 0.0f}},
      // [51] (0, 1, 0)
      {{-s, s, s, s}, {0.0f, 0.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
      // [52] (0, 0, 1)
      {{-s, -s, s, -s}, {0.0f, 0.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
      // [53] (1, 0, 1)
      {{s, -s, s, -s}, {0.0f, 0.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 1.0f}},
      // [54] (1, 1, 1)
      {{s, s, s, -s}, {0.0f, 0.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}},
      // [55] (0, 1, 1)
      {{-s, s, s, -s}, {0.0f, 0.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 1.0f}},

      // =====================================================================
      // CELL 7: Back Cell (-Z, z = -s), Normal = (0, 0, -1, 0)
      // Local axes: (u, v, w) -> (+X, +Y, +W)
      // =====================================================================
      // [56] (0, 0, 0)
      {{-s, -s, -s, -s}, {0.0f, 0.0f, -1.0f, 0.0f}, {1.0f, 0.5f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},
      // [57] (1, 0, 0)
      {{s, -s, -s, -s}, {0.0f, 0.0f, -1.0f, 0.0f}, {1.0f, 0.5f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
      // [58] (1, 1, 0)
      {{s, s, -s, -s}, {0.0f, 0.0f, -1.0f, 0.0f}, {1.0f, 0.5f, 0.0f, 1.0f}, {1.0f, 1.0f, 0.0f}},
      // [59] (0, 1, 0)
      {{-s, s, -s, -s}, {0.0f, 0.0f, -1.0f, 0.0f}, {1.0f, 0.5f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
      // [60] (0, 0, 1)
      {{-s, -s, -s, s}, {0.0f, 0.0f, -1.0f, 0.0f}, {1.0f, 0.5f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
      // [61] (1, 0, 1)
      {{s, -s, -s, s}, {0.0f, 0.0f, -1.0f, 0.0f}, {1.0f, 0.5f, 0.0f, 1.0f}, {1.0f, 0.0f, 1.0f}},
      // [62] (1, 1, 1)
      {{s, s, -s, s}, {0.0f, 0.0f, -1.0f, 0.0f}, {1.0f, 0.5f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}},
      // [63] (0, 1, 1)
      {{-s, s, -s, s}, {0.0f, 0.0f, -1.0f, 0.0f}, {1.0f, 0.5f, 0.0f, 1.0f}, {0.0f, 1.0f, 1.0f}},
  };

  // Build clockwise triangle indices for the 6 square faces of each of the 8 cubic cells
  indices.reserve(8 * 36);
  for (uint16_t cell = 0; cell < 8; ++cell) {
    uint16_t b = cell * 8;
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
    indices.insert(indices.end(), std::begin(cellIndices), std::end(cellIndices));
  }

  generateTexture3D(texDim);
}

void Hypercube::generateTexture3D(uint32_t dim) {
  if (dim == 0) {
    dim = 16;
  }
  texWidth = dim;
  texHeight = dim;
  texDepth = dim;
  textureData3D.resize(static_cast<size_t>(texWidth) * texHeight * texDepth * 4);

  for (uint32_t z = 0; z < texDepth; ++z) {
    float wNorm = (texDepth > 1) ? static_cast<float>(z) / static_cast<float>(texDepth - 1) : 0.0f;
    bool zBorder = (z == 0 || z == texDepth - 1);

    for (uint32_t y = 0; y < texHeight; ++y) {
      float vNorm = (texHeight > 1) ? static_cast<float>(y) / static_cast<float>(texHeight - 1) : 0.0f;
      bool yBorder = (y == 0 || y == texHeight - 1);

      for (uint32_t x = 0; x < texWidth; ++x) {
        float uNorm = (texWidth > 1) ? static_cast<float>(x) / static_cast<float>(texWidth - 1) : 0.0f;
        bool xBorder = (x == 0 || x == texWidth - 1);

        size_t idx = ((static_cast<size_t>(z) * texHeight + y) * texWidth + x) * 4;

        // Highlight 3D cubic cell edges where at least two coordinates lie on the boundary
        int borderCount = (xBorder ? 1 : 0) + (yBorder ? 1 : 0) + (zBorder ? 1 : 0);
        if (borderCount >= 2) {
          textureData3D[idx + 0] = 255;
          textureData3D[idx + 1] = 255;
          textureData3D[idx + 2] = 255;
          textureData3D[idx + 3] = 255;
        } else {
          // 3D checkerboard modulated by (u, v, w) color gradient
          uint32_t checker = ((x / 2) + (y / 2) + (z / 2)) & 1u;
          float shade = checker ? 1.0f : 0.65f;

          uint8_t r = static_cast<uint8_t>(std::clamp((0.25f + 0.75f * uNorm) * shade * 255.0f, 0.0f, 255.0f));
          uint8_t g = static_cast<uint8_t>(std::clamp((0.25f + 0.75f * vNorm) * shade * 255.0f, 0.0f, 255.0f));
          uint8_t b = static_cast<uint8_t>(std::clamp((0.25f + 0.75f * wNorm) * shade * 255.0f, 0.0f, 255.0f));

          textureData3D[idx + 0] = r;
          textureData3D[idx + 1] = g;
          textureData3D[idx + 2] = b;
          textureData3D[idx + 3] = 255;
        }
      }
    }
  }
}

glm::vec4 Hypercube::sampleTexture3D(const glm::vec3 &uvw) const {
  if (textureData3D.empty() || texWidth == 0 || texHeight == 0 || texDepth == 0) {
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
      textureData3D[idx + 0] / 255.0f,
      textureData3D[idx + 1] / 255.0f,
      textureData3D[idx + 2] / 255.0f,
      textureData3D[idx + 3] / 255.0f);
}

glm::vec3 Hypercube::projectVertexTo3D(size_t vertexIndex, float viewerW) const {
  const glm::vec4 &p = Data.at(vertexIndex).pos;
  float denom = viewerW - p.w;
  if (std::abs(denom) < 1e-4f) {
    denom = (denom >= 0.0f) ? 1e-4f : -1e-4f;
  }
  float factor = viewerW / denom;
  return glm::vec3(p.x * factor, p.y * factor, p.z * factor);
}

Hypercube::~Hypercube() {}

