#ifndef SPHERE_H
#define SPHERE_H

#include <glm/glm.hpp>
#include <iostream>
#include <vector>

class Sphere {
public:
  struct Vertex {
    glm::vec3 pos;
    glm::vec3 normals;
    glm::vec4 colors;
    glm::vec2 texCoords;
  };

  typedef struct {
    int a;
    int b;
  } Edge;

  std::vector<Edge> edges;
  std::vector<uint16_t> indices;
  std::vector<Vertex> Data;
  std::vector<glm::vec2> texCoords;

  float phi;
  float dphi;
  float theta;
  float dtheta;
  int hseg;
  int vseg;
  int subdivisions;
  float radius;
  float centerX;
  float centerY;
  float centerZ;

  struct HexGridConfig {
    int frequency;      // Subdivision frequency n
    int totalTiles;     // 10 * n^2 + 2
    int hexagonCount;   // 10 * n^2 - 10
    int pentagonCount;  // 12
  };

  struct HexTile {
    uint32_t id;
    glm::vec3 center;
    glm::vec3 normal;
    bool isPentagon;
    std::vector<int32_t> neighbors;          // 5 or 6 neighbor tile IDs in cyclic order
    std::vector<glm::vec3> boundaryVertices; // 5 or 6 corner positions in cyclic order
    glm::vec4 color;
    glm::vec4 baseColor;                     // Original terrain color
    int terrainType;                         // 0: Ocean, 1: Plains, 2: Mountains, etc.
    uint32_t vertexStart;                    // First vertex index in Data
    uint32_t vertexCount;                    // Number of vertices for this tile
  };

  std::vector<HexTile> hexTiles;
  int hexFrequency;

  Sphere(float radius = 1.0f, int subdivisions = 3);
  Sphere(float centerX, float centerY, float centerZ, float radius = 1.0f,
         int subdivisions = 3);
  Sphere(float radius, int hseg, int vseg);
  Sphere(float centerX, float centerY, float centerZ, float radius,
         int hseg, int vseg);
  ~Sphere();

  void UVSphere();
  void generateIcosahedron(float size);
  void generateIcosasphere(float radius, int subdivisions = 3);
  void generateIcosasphere(float cx, float cy, float cz, float radius,
                           int subdivisions = 3);

  // Hexagonal sphere generation & tile selection
  static HexGridConfig calculateHexGridConfig(int desiredHexagons);
  void generateHexasphere(float radius, int targetHexagons, float tileInset = 0.95f);
  void generateHexasphere(float cx, float cy, float cz, float radius,
                          int targetHexagons, float tileInset = 0.95f);
  int findClosestTile(const glm::vec3 &worldPos) const;
  void setTileColor(int tileId, const glm::vec4 &color, void *mappedGpuBuffer = nullptr);
  void resetTileColor(int tileId, void *mappedGpuBuffer = nullptr);

  float getRadius() const;
  void setRadius(float r);
  int getSubdivisions() const;
  void setSubdivisions(int subs);
};

#endif