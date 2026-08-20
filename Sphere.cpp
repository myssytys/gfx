#include "Sphere.h"

#include <algorithm>
#include <cmath>

#define PI 3.14159265358979323846f

Sphere::Sphere(float radius, int hseg, int vseg)
    : phi(0.0f), dphi(0.0f), theta(0.0f), dtheta(0.0f), hseg(hseg), vseg(vseg),
      radius(radius), centerX(0.0f), centerY(0.0f), centerZ(0.0f) {
  UVSphere();
}

Sphere::Sphere(float centerX, float centerY, float centerZ, float radius,
               int hseg, int vseg)
    : phi(0.0f), dphi(0.0f), theta(0.0f), dtheta(0.0f), hseg(hseg), vseg(vseg),
      radius(radius), centerX(centerX), centerY(centerY), centerZ(centerZ) {
  UVSphere();
}

Sphere::~Sphere() {}

void Sphere::UVSphere() {
  Data.clear();
  indices.clear();
  texCoords.clear();

  if (hseg < 3)
    hseg = 3;
  if (vseg < 2)
    vseg = 2;

  dtheta = 2.0f * PI / static_cast<float>(hseg);
  dphi = PI / static_cast<float>(vseg);

  // Generate vertices (vseg + 1 rows, hseg + 1 columns)
  for (int i = 0; i <= vseg; ++i) {
    float latFraction = static_cast<float>(i) / static_cast<float>(vseg);
    float currentPhi = latFraction * PI; // 0 at north pole (+Y) to PI at south pole (-Y)
    float v = 1.0f - latFraction;        // 1.0 at north pole (+Y), 0.0 at south pole (-Y)

    float sinPhi = std::sin(currentPhi);
    float cosPhi = std::cos(currentPhi);

    for (int j = 0; j <= hseg; ++j) {
      float u = static_cast<float>(j) / static_cast<float>(hseg);
      float currentTheta = u * 2.0f * PI; // 0 to 2*PI

      float sinTheta = std::sin(currentTheta);
      float cosTheta = std::cos(currentTheta);

      glm::vec3 normal(sinPhi * cosTheta, cosPhi, sinPhi * sinTheta);
      glm::vec3 pos = glm::vec3(centerX, centerY, centerZ) + normal * radius;
      glm::vec4 color(1.0f, 1.0f, 1.0f, 1.0f);
      glm::vec2 texCoord(u, v);

      texCoords.push_back(texCoord);
      Data.push_back({pos, normal, color, texCoord});
    }
  }

  // Generate indices with Clockwise winding order for Vulkan
  for (int i = 0; i < vseg; ++i) {
    for (int j = 0; j < hseg; ++j) {
      uint16_t current = static_cast<uint16_t>(i * (hseg + 1) + j);
      uint16_t next = static_cast<uint16_t>((i + 1) * (hseg + 1) + j);

      // Top triangle (omitted at north pole where current & current+1 coincide)
      if (i != 0) {
        indices.push_back(current);
        indices.push_back(static_cast<uint16_t>(current + 1));
        indices.push_back(static_cast<uint16_t>(next + 1));
      }

      // Bottom triangle (omitted at south pole where next & next+1 coincide)
      if (i != vseg - 1) {
        indices.push_back(current);
        indices.push_back(static_cast<uint16_t>(next + 1));
        indices.push_back(next);
      }
    }
  }
}

void Sphere::generateIcosahedron(float size) {
  Data.clear();
  indices.clear();
  edges.clear();
  texCoords.clear();

  float icosahedronPhi = (1.0f + std::sqrt(5.0f)) / 2.0f;

  std::vector<glm::vec3> basePositions = {
      {-1.0f, icosahedronPhi, 0.0f},  {1.0f, icosahedronPhi, 0.0f},
      {-1.0f, -icosahedronPhi, 0.0f}, {1.0f, -icosahedronPhi, 0.0f},
      {0.0f, -1.0f, icosahedronPhi},  {0.0f, 1.0f, icosahedronPhi},
      {0.0f, -1.0f, -icosahedronPhi}, {0.0f, 1.0f, -icosahedronPhi},
      {icosahedronPhi, 0.0f, -1.0f},  {icosahedronPhi, 0.0f, 1.0f},
      {-icosahedronPhi, 0.0f, -1.0f}, {-icosahedronPhi, 0.0f, 1.0f}};

  for (const auto &pos : basePositions) {
    glm::vec3 normal = glm::normalize(pos);
    glm::vec3 p = glm::vec3(centerX, centerY, centerZ) + normal * size;
    glm::vec4 color(1.0f, 1.0f, 1.0f, 1.0f);
    float u = 0.5f + std::atan2(normal.z, normal.x) / (2.0f * PI);
    float v = 0.5f + std::asin(std::clamp(normal.y, -1.0f, 1.0f)) / PI;
    glm::vec2 texCoord(u, v);

    texCoords.push_back(texCoord);
    Data.push_back({p, normal, color, texCoord});
  }

  // 20 triangular faces with clockwise winding order
  indices = {0, 5, 11, 0, 1,  5,  0,  7,  1,  0,  10, 7, 0, 11, 10,
             1, 9, 5,  5, 4,  11, 11, 2,  10, 10, 6,  7, 7, 8,  1,
             3, 4, 9,  3, 2,  4,  3,  6,  2,  3,  8,  6, 3, 9,  8,
             4, 5, 9,  2, 11, 4,  6,  10, 2,  8,  7,  6, 9, 1,  8};

  // 30 unique edges
  edges = {{0, 1}, {0, 5},  {0, 7}, {0, 10}, {0, 11}, {1, 5},  {1, 7},  {1, 8},
           {1, 9}, {2, 3},  {2, 4}, {2, 6},  {2, 10}, {2, 11}, {3, 4},  {3, 6},
           {3, 8}, {3, 9},  {4, 5}, {4, 9},  {4, 11}, {5, 9},  {5, 11}, {6, 7},
           {6, 8}, {6, 10}, {7, 8}, {7, 10}, {8, 9},  {10, 11}};
}

float Sphere::getRadius() const { return radius; }

void Sphere::setRadius(float r) {
  radius = r;
  UVSphere();
}
