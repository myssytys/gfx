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
  float radius;
  float centerX;
  float centerY;
  float centerZ;

  Sphere(float radius = 1.0f, int hseg = 32, int vseg = 32);
  Sphere(float centerX, float centerY, float centerZ, float radius = 1.0f,
         int hseg = 32, int vseg = 32);
  ~Sphere();

  void UVSphere();
  void generateIcosahedron(float size);
  float getRadius() const;
  void setRadius(float r);
};

#endif