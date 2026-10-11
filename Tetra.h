#ifndef TETRA_H
#define TETRA_H

#include <glm/glm.hpp>
#include <iostream>
#include <vector>

class Tetra {

public:
  struct Vertex {
    glm::vec3 pos;
    glm::vec3 normals;
    glm::vec4 colors;
    glm::vec2 texCoords;
  };

  float edge;
  std::vector<uint16_t> indices;
  std::vector<Vertex> Data;

  Tetra(float edge = 1.0f);
  ~Tetra();
};

#endif