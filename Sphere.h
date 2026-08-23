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
  float getRadius() const;
  void setRadius(float r);
  int getSubdivisions() const;
  void setSubdivisions(int subs);
};

#endif