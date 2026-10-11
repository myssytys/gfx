// CUBE CLASS CLOCKWISE WINDING ORDER

#include "Cube.h"

Cube::Cube(float size) {

  std::cout << "Test! -><-";
  Data = {

      /* 		{-1, -1, -1}, {1, -1, -1}, {1, 1, -1}, {-1, 1, -1},
              {-1, -1, 1}, {1, -1, 1}, {1, 1, 1}, {-1, 1, 1}*/

      // Vertices                				 // Normals //
      // Colors+Alpha     // Texcoord U+V front face
      // Right-Down [0]
      {{1.0f * size, -1.0f * size, 1.0f * size},
       {0.0f, 0.0f, 1.0f},
       {1.0f, 1.0f, 1.0f, 1.0f},
       {1.0f, 0.0f}},
      // Left-Down [1]
      {{-1.0f * size, -1.0f * size, 1.0f * size},
       {0.0f, 0.0f, 1.0f},
       {1.0f, 1.0f, 1.0f, 1.0f},
       {0.0f, 0.0f}},
      // Left-Up [2]
      {{-1.0f * size, 1.0f * size, 1.0f * size},
       {0.0f, 0.0f, 1.0f},
       {1.0f, 1.0f, 1.0f, 1.0f},
       {0.0f, 1.0f}},
      // Right-Up [3]
      {{1.0f * size, 1.0f * size, 1.0f * size},
       {0.0f, 0.0f, 1.0f},
       {1.0f, 1.0f, 1.0f, 1.0f},
       {1.0f, 1.0f}},
      // back face
      // Left Down [4]
      {{-1.0f * size, -1.0f * size, -1.0f * size},
       {0.0f, 0.0f, -1.0f},
       {1.0f, 1.0f, 1.0f, 1.0f},
       {1.0f, 0.0f}},
      // Right-Down [5]
      {{1.0f * size, -1.0f * size, -1.0f * size},
       {0.0f, 0.0f, -1.0f},
       {1.0f, 1.0f, 1.0f, 1.0f},
       {0.0f, 0.0f}},
      // RIGHT UP [6]
      {{1.0f * size, 1.0f * size, -1.0f * size},
       {0.0f, 0.0f, -1.0f},
       {1.0f, 1.0f, 1.0f, 1.0f},
       {0.0f, 1.0f}},
      // LEFT UP [7]
      {{-1.0f * size, 1.0f * size, -1.0f * size},
       {0.0f, 0.0f, -1.0f},
       {1.0f, 1.0f, 1.0f, 1.0f},
       {1.0f, 1.0f}},

      // left face

      // Z-UP Y-UP[8]
      {{-1.0f * size, size * 1.0f, 1.0f * size},
       {-1.0f, 0.0f, 0.0f},
       {1.0f, 1.0f, 1.0f, 1.0f},
       {1.0f, 1.0f}},
      // Z-DOWN Y-UP[9]
      {{-1.0f * size, size * 1.0f, -1.0f * size},
       {-1.0f, 0.0f, 0.0f},
       {1.0f, 1.0f, 1.0f, 1.0f},
       {0.0f, 1.0f}},
      // Z Down Y-DOWN [10]
      {{-1.0f * size, size * -1.0f, -1.0f * size},
       {-1.0f, 0.0f, 0.0f},
       {1.0f, 1.0f, 1.0f, 1.0f},
       {0.0f, 0.0f}},
      // Z-UP  Y-DOWN[11]
      {{-1.0f * size, size * -1.0f, 1.0f * size},
       {-1.0f, 0.0f, 0.0f},
       {1.0f, 1.0f, 1.0f, 1.0f},
       {1.0f, 0.0f}},

      // right face

      // Z-DOWN Y-DOWN [12]
      {{1.0f * size, -1.0f * size, -1.0f * size},
       {1.0f, 0.0f, 0.0f},
       {1.0f, 0.0f, 0.0f, 1.0f},
       {1.0f, 0.0f}},
      // Z-UP Y-DOWN [13]
      {{1.0f * size, -1.0f * size, 1.0f * size},
       {1.0f, 0.0f, 0.0f},
       {1.0f, 0.0f, 0.0f, 1.0f},
       {0.0f, 0.0f}},
      // Z UP-YUP- [14]
      {{1.0f * size, 1.0f * size, 1.0f * size},
       {1.0f, 0.0f, 0.0f},
       {1.0f, 0.0f, 0.0f, 1.0f},
       {0.0f, 1.0f}},
      // Z DOWN - YUP [15]
      {{1.0f * size, 1.0f * size, -1.0f * size},
       {1.0f, 0.0f, 0.0f},
       {1.0f, 0.0f, 0.0f, 1.0f},
       {1.0f, 1.0f}},
      // top face
      // XRIGHT Z-UP [16]
      {{1.0f * size, 1.0f * size, 1.0f * size},
       {0.0f, 1.0f, 0.0f},
       {0.0f, 0.0f, 1.0f, 1.0f},
       {1.0f, 0.0f}},
      // XLEFT Z-UP [17]
      {{-1.0f * size, 1.0f * size, 1.0f * size},
       {0.0f, 1.0f, 0.0f},
       {0.0f, 0.0f, 1.0f, 1.0f},
       {0.0f, 0.0f}},
      // XLEFT Z-DOWN [18]
      {{-1.0f * size, 1.0f * size, -1.0f * size},
       {0.0f, 1.0f, 0.0f},
       {0.0f, 0.0f, 1.0f, 1.0f},
       {0.0f, 1.0f}},
      // XRIGHT Z-DOWN [19]
      {{1.0f * size, 1.0f * size, -1.0f * size},
       {0.0f, 1.0f, 0.0f},
       {0.0f, 0.0f, 1.0f, 1.0f},
       {1.0f, 1.0f}},
      // bottom face
      // XRIGHT ZUP [20]
      {{1.0f * size, -1.0f * size, 1.0f * size},
       {0.0f, -1.0f, 0.0f},
       {0.0f, 1.0f, 1.0f, 1.0f},
       {0.0f, 0.0f}},
      // XLEFT ZUP [21]
      {{-1.0f * size, -1.0f * size, 1.0f * size},
       {0.0f, -1.0f, 0.0f},
       {0.0f, 1.0f, 1.0f, 1.0f},
       {1.0f, 0.0f}},
      // XLEFT ZDOWN [22]
      {{-1.0f * size, -1.0f * size, -1.0f * size},
       {0.0f, -1.0f, 0.0f},
       {0.0f, 1.0f, 1.0f, 1.0f},
       {1.0f, 1.0f}},
      // XRIGHT ZDOWN [23]
      {{1.0f * size, -1.0f * size, -1.0f * size},
       {0.0f, -1.0f, 0.0f},
       {0.0f, 1.0f, 1.0f, 1.0f},
       {0.0f, 1.0f}},

  };

  std::cout << "Data size: " << Data.size();

  indices = {// Front face
             0, 1, 2, 2, 3, 0,
             // Back face
             4, 5, 6, 6, 7, 4,
             // Left face
             8, 9, 10, 10, 11, 8,
             // Right face
             12, 13, 14, 14, 15, 12,
             // Top face
             16, 17, 18, 18, 19, 16,
             // Bottom face
             20, 21, 22, 22, 23, 20};

  std::cout << "Indices Size: " << indices.size();
}

Cube::~Cube() {}
