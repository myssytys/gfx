// OCTAHEDRON CLASS CLOCKWISE WINDING ORDER

#include "Octa.h"

Octa::Octa(float size) {
  this->size = size;

  const float norm = 0.57735027f; // 1.0f / sqrt(3.0f)

  Data = {// --- TOP PYRAMID ---

          // Top-front-right face (+X, +Y, +Z)
          // Top Apex [0]
          {{0.0f, 1.0f * size, 0.0f},
           {norm, norm, norm},
           {1.0f, 1.0f, 1.0f, 1.0f},
           {0.5f, 1.0f}},
          // Right [1]
          {{1.0f * size, 0.0f, 0.0f},
           {norm, norm, norm},
           {1.0f, 1.0f, 1.0f, 1.0f},
           {1.0f, 0.0f}},
          // Front [2]
          {{0.0f, 0.0f, 1.0f * size},
           {norm, norm, norm},
           {1.0f, 1.0f, 1.0f, 1.0f},
           {0.0f, 0.0f}},

          // Top-front-left face (-X, +Y, +Z)
          // Top Apex [3]
          {{0.0f, 1.0f * size, 0.0f},
           {-norm, norm, norm},
           {1.0f, 1.0f, 1.0f, 1.0f},
           {0.5f, 1.0f}},
          // Front [4]
          {{0.0f, 0.0f, 1.0f * size},
           {-norm, norm, norm},
           {1.0f, 1.0f, 1.0f, 1.0f},
           {1.0f, 0.0f}},
          // Left [5]
          {{-1.0f * size, 0.0f, 0.0f},
           {-norm, norm, norm},
           {1.0f, 1.0f, 1.0f, 1.0f},
           {0.0f, 0.0f}},

          // Top-back-left face (-X, +Y, -Z)
          // Top Apex [6]
          {{0.0f, 1.0f * size, 0.0f},
           {-norm, norm, -norm},
           {1.0f, 1.0f, 1.0f, 1.0f},
           {0.5f, 1.0f}},
          // Left [7]
          {{-1.0f * size, 0.0f, 0.0f},
           {-norm, norm, -norm},
           {1.0f, 1.0f, 1.0f, 1.0f},
           {1.0f, 0.0f}},
          // Back [8]
          {{0.0f, 0.0f, -1.0f * size},
           {-norm, norm, -norm},
           {1.0f, 1.0f, 1.0f, 1.0f},
           {0.0f, 0.0f}},

          // Top-back-right face (+X, +Y, -Z)
          // Top Apex [9]
          {{0.0f, 1.0f * size, 0.0f},
           {norm, norm, -norm},
           {1.0f, 1.0f, 1.0f, 1.0f},
           {0.5f, 1.0f}},
          // Back [10]
          {{0.0f, 0.0f, -1.0f * size},
           {norm, norm, -norm},
           {1.0f, 1.0f, 1.0f, 1.0f},
           {1.0f, 0.0f}},
          // Right [11]
          {{1.0f * size, 0.0f, 0.0f},
           {norm, norm, -norm},
           {1.0f, 1.0f, 1.0f, 1.0f},
           {0.0f, 0.0f}},

          // --- BOTTOM PYRAMID ---

          // Bottom-front-right face (+X, -Y, +Z)
          // Bottom Apex [12]
          {{0.0f, -1.0f * size, 0.0f},
           {norm, -norm, norm},
           {1.0f, 1.0f, 1.0f, 1.0f},
           {0.5f, 1.0f}},
          // Front [13]
          {{0.0f, 0.0f, 1.0f * size},
           {norm, -norm, norm},
           {1.0f, 1.0f, 1.0f, 1.0f},
           {1.0f, 0.0f}},
          // Right [14]
          {{1.0f * size, 0.0f, 0.0f},
           {norm, -norm, norm},
           {1.0f, 1.0f, 1.0f, 1.0f},
           {0.0f, 0.0f}},

          // Bottom-front-left face (-X, -Y, +Z)
          // Bottom Apex [15]
          {{0.0f, -1.0f * size, 0.0f},
           {-norm, -norm, norm},
           {1.0f, 1.0f, 1.0f, 1.0f},
           {0.5f, 1.0f}},
          // Left [16]
          {{-1.0f * size, 0.0f, 0.0f},
           {-norm, -norm, norm},
           {1.0f, 1.0f, 1.0f, 1.0f},
           {1.0f, 0.0f}},
          // Front [17]
          {{0.0f, 0.0f, 1.0f * size},
           {-norm, -norm, norm},
           {1.0f, 1.0f, 1.0f, 1.0f},
           {0.0f, 0.0f}},

          // Bottom-back-left face (-X, -Y, -Z)
          // Bottom Apex [18]
          {{0.0f, -1.0f * size, 0.0f},
           {-norm, -norm, -norm},
           {1.0f, 1.0f, 1.0f, 1.0f},
           {0.5f, 1.0f}},
          // Back [19]
          {{0.0f, 0.0f, -1.0f * size},
           {-norm, -norm, -norm},
           {1.0f, 1.0f, 1.0f, 1.0f},
           {1.0f, 0.0f}},
          // Left [20]
          {{-1.0f * size, 0.0f, 0.0f},
           {-norm, -norm, -norm},
           {1.0f, 1.0f, 1.0f, 1.0f},
           {0.0f, 0.0f}},

          // Bottom-back-right face (+X, -Y, -Z)
          // Bottom Apex [21]
          {{0.0f, -1.0f * size, 0.0f},
           {norm, -norm, -norm},
           {1.0f, 1.0f, 1.0f, 1.0f},
           {0.5f, 1.0f}},
          // Right [22]
          {{1.0f * size, 0.0f, 0.0f},
           {norm, -norm, -norm},
           {1.0f, 1.0f, 1.0f, 1.0f},
           {1.0f, 0.0f}},
          // Back [23]
          {{0.0f, 0.0f, -1.0f * size},
           {norm, -norm, -norm},
           {1.0f, 1.0f, 1.0f, 1.0f},
           {0.0f, 0.0f}}};

  indices = {// Top-front-right face
             0, 1, 2,
             // Top-front-left face
             3, 4, 5,
             // Top-back-left face
             6, 7, 8,
             // Top-back-right face
             9, 10, 11,
             // Bottom-front-right face
             12, 13, 14,
             // Bottom-front-left face
             15, 16, 17,
             // Bottom-back-left face
             18, 19, 20,
             // Bottom-back-right face
             21, 22, 23};
}

Octa::~Octa() {}
