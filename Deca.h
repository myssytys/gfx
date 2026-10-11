#ifndef DECA_H
#define DECA_H

#include <glm/glm.hpp>
#include <iostream>
#include <vector>

class Deca {

public:
  struct Vertex {
    glm::vec3 pos;
    glm::vec3 normals;
    glm::vec4 colors;
    glm::vec2 texCoords;
  };

  float size;
  std::vector<uint16_t> indices;
  std::vector<Vertex> Data;

  Deca(float size = 1.0f);
  ~Deca();
};

#endif
