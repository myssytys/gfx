#include "Sphere.h"

#include <algorithm>
#include <cmath>
#include <map>
#include <set>
#include <utility>

#define PI 3.14159265358979323846f

Sphere::Sphere(float radius, int subdivisions)
    : phi(0.0f), dphi(0.0f), theta(0.0f), dtheta(0.0f), hseg(32), vseg(32),
      subdivisions(subdivisions), radius(radius), centerX(0.0f), centerY(0.0f),
      centerZ(0.0f) {
  generateIcosasphere(radius, subdivisions);
}

Sphere::Sphere(float centerX, float centerY, float centerZ, float radius,
               int subdivisions)
    : phi(0.0f), dphi(0.0f), theta(0.0f), dtheta(0.0f), hseg(32), vseg(32),
      subdivisions(subdivisions), radius(radius), centerX(centerX),
      centerY(centerY), centerZ(centerZ) {
  generateIcosasphere(radius, subdivisions);
}

Sphere::Sphere(float radius, int hseg, int vseg)
    : phi(0.0f), dphi(0.0f), theta(0.0f), dtheta(0.0f), hseg(hseg), vseg(vseg),
      subdivisions(3), radius(radius), centerX(0.0f), centerY(0.0f),
      centerZ(0.0f) {
  subdivisions = (hseg <= 6 && hseg >= 0) ? hseg : 3;
  generateIcosasphere(radius, subdivisions);
}

Sphere::Sphere(float centerX, float centerY, float centerZ, float radius,
               int hseg, int vseg)
    : phi(0.0f), dphi(0.0f), theta(0.0f), dtheta(0.0f), hseg(hseg), vseg(vseg),
      subdivisions(3), radius(radius), centerX(centerX), centerY(centerY),
      centerZ(centerZ) {
  subdivisions = (hseg <= 6 && hseg >= 0) ? hseg : 3;
  generateIcosasphere(radius, subdivisions);
}

Sphere::~Sphere() {}

void Sphere::UVSphere() {
  Data.clear();
  indices.clear();
  edges.clear();
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
    float currentPhi =
        latFraction * PI; // 0 at north pole (+Y) to PI at south pole (-Y)
    float v =
        1.0f - latFraction; // 1.0 at north pole (+Y), 0.0 at south pole (-Y)

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
  generateIcosasphere(centerX, centerY, centerZ, size, 0);
}

void Sphere::generateIcosasphere(float radius, int subdivisions) {
  generateIcosasphere(centerX, centerY, centerZ, radius, subdivisions);
}

void Sphere::generateIcosasphere(float cx, float cy, float cz, float radius,
                                 int subdivisions) {
  Data.clear();
  indices.clear();
  edges.clear();
  texCoords.clear();

  this->centerX = cx;
  this->centerY = cy;
  this->centerZ = cz;
  this->radius = radius;
  this->subdivisions = std::clamp(subdivisions, 0, 6);

  float icosahedronPhi = (1.0f + std::sqrt(5.0f)) / 2.0f;

  std::vector<glm::vec3> unitPositions = {
      {-1.0f, icosahedronPhi, 0.0f},  {1.0f, icosahedronPhi, 0.0f},
      {-1.0f, -icosahedronPhi, 0.0f}, {1.0f, -icosahedronPhi, 0.0f},
      {0.0f, -1.0f, icosahedronPhi},  {0.0f, 1.0f, icosahedronPhi},
      {0.0f, -1.0f, -icosahedronPhi}, {0.0f, 1.0f, -icosahedronPhi},
      {icosahedronPhi, 0.0f, -1.0f},  {icosahedronPhi, 0.0f, 1.0f},
      {-icosahedronPhi, 0.0f, -1.0f}, {-icosahedronPhi, 0.0f, 1.0f}};

  for (auto &pos : unitPositions) {
    pos = glm::normalize(pos);
  }

  // 20 triangular faces with clockwise winding order for Vulkan
  indices = {0, 5, 11, 0, 1,  5,  0,  7,  1,  0,  10, 7, 0, 11, 10,
             1, 9, 5,  5, 4,  11, 11, 2,  10, 10, 6,  7, 7, 8,  1,
             3, 4, 9,  3, 2,  4,  3,  6,  2,  3,  8,  6, 3, 9,  8,
             4, 5, 9,  2, 11, 4,  6,  10, 2,  8,  7,  6, 9, 1,  8};

  // Subdivide triangles
  for (int sub = 0; sub < this->subdivisions; ++sub) {
    std::map<std::pair<uint16_t, uint16_t>, uint16_t> midpointCache;
    auto getMidpoint = [&](uint16_t i1, uint16_t i2) -> uint16_t {
      if (i1 > i2)
        std::swap(i1, i2);
      auto key = std::make_pair(i1, i2);
      auto it = midpointCache.find(key);
      if (it != midpointCache.end())
        return it->second;

      glm::vec3 mid =
          glm::normalize((unitPositions[i1] + unitPositions[i2]) * 0.5f);
      uint16_t newIndex = static_cast<uint16_t>(unitPositions.size());
      unitPositions.push_back(mid);
      midpointCache[key] = newIndex;
      return newIndex;
    };

    std::vector<uint16_t> newIndices;
    newIndices.reserve(indices.size() * 4);

    for (size_t i = 0; i < indices.size(); i += 3) {
      uint16_t a = indices[i];
      uint16_t b = indices[i + 1];
      uint16_t c = indices[i + 2];

      uint16_t ab = getMidpoint(a, b);
      uint16_t bc = getMidpoint(b, c);
      uint16_t ca = getMidpoint(c, a);

      // Subdivided into 4 triangles maintaining clockwise winding
      newIndices.push_back(a);
      newIndices.push_back(ab);
      newIndices.push_back(ca);

      newIndices.push_back(b);
      newIndices.push_back(bc);
      newIndices.push_back(ab);

      newIndices.push_back(c);
      newIndices.push_back(ca);
      newIndices.push_back(bc);

      newIndices.push_back(ab);
      newIndices.push_back(bc);
      newIndices.push_back(ca);
    }
    indices = std::move(newIndices);
  }

  // Populate vertex data
  Data.reserve(unitPositions.size());
  texCoords.reserve(unitPositions.size());

  for (const auto &normal : unitPositions) {
    glm::vec3 pos =
        glm::vec3(this->centerX, this->centerY, this->centerZ) + normal * radius;
    glm::vec4 color(1.0f, 1.0f, 1.0f, 1.0f);
    float u = 0.5f + std::atan2(normal.z, normal.x) / (2.0f * PI);
    float v = 0.5f + std::asin(std::clamp(normal.y, -1.0f, 1.0f)) / PI;
    glm::vec2 texCoord(u, v);

    texCoords.push_back(texCoord);
    Data.push_back({pos, normal, color, texCoord});
  }

  // Generate unique edges
  std::set<std::pair<uint16_t, uint16_t>> uniqueEdges;
  for (size_t i = 0; i < indices.size(); i += 3) {
    uint16_t v0 = indices[i];
    uint16_t v1 = indices[i + 1];
    uint16_t v2 = indices[i + 2];

    uniqueEdges.insert({std::min(v0, v1), std::max(v0, v1)});
    uniqueEdges.insert({std::min(v1, v2), std::max(v1, v2)});
    uniqueEdges.insert({std::min(v2, v0), std::max(v2, v0)});
  }

  edges.reserve(uniqueEdges.size());
  for (const auto &e : uniqueEdges) {
    edges.push_back({static_cast<int>(e.first), static_cast<int>(e.second)});
  }
}

float Sphere::getRadius() const { return radius; }

void Sphere::setRadius(float r) {
  radius = r;
  generateIcosasphere(radius, subdivisions);
}

int Sphere::getSubdivisions() const { return subdivisions; }

void Sphere::setSubdivisions(int subs) {
  subdivisions = subs;
  generateIcosasphere(radius, subdivisions);
}

