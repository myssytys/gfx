#ifndef HYPERTETRA_H
#define HYPERTETRA_H

#include <cstdint>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>

class Hypertetra {

public:
  struct Vertex {
    glm::vec4 pos;
    glm::vec4 normals;
    glm::vec4 colors;
    glm::vec3 texCoords;
  };

  float edge;
  std::vector<uint16_t> indices;
  std::vector<Vertex> Data;

  // 3D RGBA8 texture volume (texWidth * texHeight * texDepth * 4 bytes)
  uint32_t texWidth = 16;
  uint32_t texHeight = 16;
  uint32_t texDepth = 16;
  std::vector<uint8_t> textureData3D;

  Hypertetra(float edge = 1.0f, uint32_t texDim = 16);
  ~Hypertetra();

  void generateTexture3D(uint32_t dim = 16);
  glm::vec4 sampleTexture3D(const glm::vec3 &uvw) const;
  glm::vec3 projectVertexTo3D(size_t vertexIndex, float viewerW = 3.0f) const;
};

using Tetra4D = Hypertetra;

#endif

