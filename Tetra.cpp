// TETRAHEDRON CLASS CLOCKWISE WINDING ORDER

#include "Tetra.h"

Tetra::Tetra(float edge) {
  this->edge = edge;

  Data = {
      // Right-front face
      // Apex [0]
      {{0.0f, 1.0f * edge, 0.0f},
       {0.904534f, 0.301511f, 0.301511f},
       {1.0f, 1.0f, 1.0f, 1.0f},
       {0.5f, 1.0f}},
      // Base-Right [1]
      {{edge / 2.0f, -edge / 2.0f, 0.0f},
       {0.904534f, 0.301511f, 0.301511f},
       {1.0f, 1.0f, 1.0f, 1.0f},
       {1.0f, 0.0f}},
      // Front [2]
      {{0.0f, 0.0f, 1.0f * edge},
       {0.904534f, 0.301511f, 0.301511f},
       {1.0f, 1.0f, 1.0f, 1.0f},
       {0.0f, 0.0f}},

      // Left-front face
      // Apex [3]
      {{0.0f, 1.0f * edge, 0.0f},
       {-0.904534f, 0.301511f, 0.301511f},
       {1.0f, 1.0f, 1.0f, 1.0f},
       {0.5f, 1.0f}},
      // Front [4]
      {{0.0f, 0.0f, 1.0f * edge},
       {-0.904534f, 0.301511f, 0.301511f},
       {1.0f, 1.0f, 1.0f, 1.0f},
       {1.0f, 0.0f}},
      // Base-Left [5]
      {{-edge / 2.0f, -edge / 2.0f, 0.0f},
       {-0.904534f, 0.301511f, 0.301511f},
       {1.0f, 1.0f, 1.0f, 1.0f},
       {0.0f, 0.0f}},

      // Back face
      // Apex [6]
      {{0.0f, 1.0f * edge, 0.0f},
       {0.0f, 0.0f, -1.0f},
       {1.0f, 1.0f, 1.0f, 1.0f},
       {0.5f, 1.0f}},
      // Base-Right [7]
      {{edge / 2.0f, -edge / 2.0f, 0.0f},
       {0.0f, 0.0f, -1.0f},
       {1.0f, 1.0f, 1.0f, 1.0f},
       {0.0f, 0.0f}},
      // Base-Left [8]
      {{-edge / 2.0f, -edge / 2.0f, 0.0f},
       {0.0f, 0.0f, -1.0f},
       {1.0f, 1.0f, 1.0f, 1.0f},
       {1.0f, 0.0f}},

      // Bottom face
      // Base-Right [9]
      {{edge / 2.0f, -edge / 2.0f, 0.0f},
       {0.0f, -0.894427f, 0.447214f},
       {1.0f, 1.0f, 1.0f, 1.0f},
       {1.0f, 0.0f}},
      // Base-Left [10]
      {{-edge / 2.0f, -edge / 2.0f, 0.0f},
       {0.0f, -0.894427f, 0.447214f},
       {1.0f, 1.0f, 1.0f, 1.0f},
       {0.0f, 0.0f}},
      // Front [11]
      {{0.0f, 0.0f, 1.0f * edge},
       {0.0f, -0.894427f, 0.447214f},
       {1.0f, 1.0f, 1.0f, 1.0f},
       {0.5f, 1.0f}}};

  indices = {// Right-front face
             0, 1, 2,
             // Left-front face
             3, 4, 5,
             // Back face
             6, 7, 8,
             // Bottom face
             9, 10, 11};
}

Tetra::~Tetra() {}
