#ifndef PENTERACT_H
#define PENTERACT_H

#include <cstdint>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>

class Penteract {

public:
  struct Vertex {
    glm::vec4 pos;       // First 4 coordinates (x, y, z, w) in 5D (or projected 4D/3D)
    glm::vec4 normals;   // First 4 normal components (nx, ny, nz, nw)
    glm::vec4 colors;    // RGBA color
    glm::vec3 texCoords; // 3D texture coordinates (u, v, w) in [0, 1]^3
    glm::vec2 fifthDim;  // x = 5th position coordinate (v), y = 5th normal component (nv)
  };

  float size;
  std::vector<uint16_t> indices;
  std::vector<Vertex> Data;

  // Projected 3D vertices convenience buffer (updated via projectAllTo3D)
  std::vector<glm::vec3> projected3D;

  // 3D RGBA8 texture volume (texWidth * texHeight * texDepth * 4 bytes)
  uint32_t texWidth = 16;
  uint32_t texHeight = 16;
  uint32_t texDepth = 16;
  std::vector<uint8_t> textureData3D;

  Penteract(float size = 1.0f, uint32_t texDim = 16);
  ~Penteract();

  void generateTexture3D(uint32_t dim = 16);
  glm::vec4 sampleTexture3D(const glm::vec3 &uvw) const;

  // Project a single 5D vertex (x, y, z, w, v) -> 4D (x, y, z, w) -> 3D (x, y, z)
  glm::vec4 projectVertexTo4D(size_t vertexIndex, float viewerV = 3.0f) const;
  glm::vec3 projectVertexTo3D(size_t vertexIndex, float viewerV = 3.0f,
                              float viewerW = 3.0f) const;
  void projectAllTo3D(float viewerV = 3.0f, float viewerW = 3.0f);
};

using Cube5D = Penteract;
using Hypercube5D = Penteract;

#endif

