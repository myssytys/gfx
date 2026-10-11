#include "Sphere.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <map>
#include <set>
#include <utility>

#define PI 3.14159265358979323846f

Sphere::Sphere(float radius, int subdivisions)
    : phi(0.0f), dphi(0.0f), theta(0.0f), dtheta(0.0f), hseg(32), vseg(32),
      subdivisions(subdivisions), radius(radius), centerX(0.0f), centerY(0.0f),
      centerZ(0.0f), hexFrequency(0) {
  generateIcosasphere(radius, subdivisions);
}

Sphere::Sphere(float centerX, float centerY, float centerZ, float radius,
               int subdivisions)
    : phi(0.0f), dphi(0.0f), theta(0.0f), dtheta(0.0f), hseg(32), vseg(32),
      subdivisions(subdivisions), radius(radius), centerX(centerX),
      centerY(centerY), centerZ(centerZ), hexFrequency(0) {
  generateIcosasphere(radius, subdivisions);
}

Sphere::Sphere(float radius, int hseg, int vseg)
    : phi(0.0f), dphi(0.0f), theta(0.0f), dtheta(0.0f), hseg(hseg), vseg(vseg),
      subdivisions(3), radius(radius), centerX(0.0f), centerY(0.0f),
      centerZ(0.0f), hexFrequency(0) {
  subdivisions = (hseg <= 6 && hseg >= 0) ? hseg : 3;
  generateIcosasphere(radius, subdivisions);
}

Sphere::Sphere(float centerX, float centerY, float centerZ, float radius,
               int hseg, int vseg)
    : phi(0.0f), dphi(0.0f), theta(0.0f), dtheta(0.0f), hseg(hseg), vseg(vseg),
      subdivisions(3), radius(radius), centerX(centerX), centerY(centerY),
      centerZ(centerZ), hexFrequency(0) {
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

Sphere::HexGridConfig Sphere::calculateHexGridConfig(int desiredHexagons) {
  if (desiredHexagons <= 0) desiredHexagons = 1;
  int n = std::max(1, static_cast<int>(std::round(std::sqrt((desiredHexagons + 10.0f) / 10.0f))));
  HexGridConfig config;
  config.frequency = n;
  config.totalTiles = 10 * n * n + 2;
  config.hexagonCount = config.totalTiles - 12;
  config.pentagonCount = 12;
  return config;
}

void Sphere::generateHexasphere(float radius, int targetHexagons, float tileInset) {
  generateHexasphere(centerX, centerY, centerZ, radius, targetHexagons, tileInset);
}

void Sphere::generateHexasphere(float cx, float cy, float cz, float radius,
                                int targetHexagons, float tileInset) {
  Data.clear();
  indices.clear();
  edges.clear();
  texCoords.clear();
  hexTiles.clear();

  this->centerX = cx;
  this->centerY = cy;
  this->centerZ = cz;
  this->radius = radius;

  HexGridConfig config = calculateHexGridConfig(targetHexagons);
  // Cap frequency to 15 to avoid exceeding uint16_t index limits (max n=15 -> ~2252 tiles)
  int n = std::clamp(config.frequency, 1, 15);
  this->hexFrequency = n;

  float icosahedronPhi = (1.0f + std::sqrt(5.0f)) / 2.0f;

  std::vector<glm::vec3> baseVertices = {
      {-1.0f,  icosahedronPhi, 0.0f}, { 1.0f,  icosahedronPhi, 0.0f},
      {-1.0f, -icosahedronPhi, 0.0f}, { 1.0f, -icosahedronPhi, 0.0f},
      { 0.0f, -1.0f,  icosahedronPhi}, { 0.0f,  1.0f,  icosahedronPhi},
      { 0.0f, -1.0f, -icosahedronPhi}, { 0.0f,  1.0f, -icosahedronPhi},
      { icosahedronPhi, 0.0f, -1.0f}, { icosahedronPhi, 0.0f,  1.0f},
      {-icosahedronPhi, 0.0f, -1.0f}, {-icosahedronPhi, 0.0f,  1.0f}};

  for (auto &v : baseVertices) {
    v = glm::normalize(v);
  }

  const std::vector<std::array<int, 3>> baseFaces = {
      {0, 5, 11}, {0, 1, 5}, {0, 7, 1}, {0, 10, 7}, {0, 11, 10},
      {1, 9, 5},  {5, 4, 11}, {11, 2, 10}, {10, 6, 7}, {7, 8, 1},
      {3, 4, 9},  {3, 2, 4},  {3, 6, 2},  {3, 8, 6},  {3, 9, 8},
      {4, 5, 9},  {2, 11, 4}, {6, 10, 2}, {8, 7, 6},  {9, 1, 8}};

  struct Vec3Key {
    int64_t x, y, z;
    bool operator<(const Vec3Key &o) const {
      if (x != o.x) return x < o.x;
      if (y != o.y) return y < o.y;
      return z < o.z;
    }
  };

  auto toKey = [](const glm::vec3 &v) {
    const float scale = 100000.0f;
    return Vec3Key{
      static_cast<int64_t>(std::round(v.x * scale)),
      static_cast<int64_t>(std::round(v.y * scale)),
      static_cast<int64_t>(std::round(v.z * scale))
    };
  };

  std::map<Vec3Key, uint16_t> vertexMap;
  std::vector<glm::vec3> unitPositions;

  auto getOrCreateVertex = [&](const glm::vec3 &p) -> uint16_t {
    glm::vec3 normP = glm::normalize(p);
    Vec3Key key = toKey(normP);
    auto it = vertexMap.find(key);
    if (it != vertexMap.end()) return it->second;
    uint16_t idx = static_cast<uint16_t>(unitPositions.size());
    unitPositions.push_back(normP);
    vertexMap[key] = idx;
    return idx;
  };

  std::vector<std::array<uint16_t, 3>> triangles;

  for (const auto &face : baseFaces) {
    glm::vec3 A = baseVertices[face[0]];
    glm::vec3 B = baseVertices[face[1]];
    glm::vec3 C = baseVertices[face[2]];

    std::vector<std::vector<uint16_t>> rowIndices(n + 1);
    for (int i = 0; i <= n; ++i) {
      rowIndices[i].resize(n - i + 1);
      for (int j = 0; j <= n - i; ++j) {
        int k = n - i - j;
        float wA = static_cast<float>(i) / n;
        float wB = static_cast<float>(j) / n;
        float wC = static_cast<float>(k) / n;
        rowIndices[i][j] = getOrCreateVertex(wA * A + wB * B + wC * C);
      }
    }

    for (int i = 0; i < n; ++i) {
      for (int j = 0; j < n - i; ++j) {
        triangles.push_back({rowIndices[i][j], rowIndices[i + 1][j], rowIndices[i][j + 1]});
        if (j + 1 <= n - i - 1) {
          triangles.push_back({rowIndices[i + 1][j], rowIndices[i + 1][j + 1], rowIndices[i][j + 1]});
        }
      }
    }
  }

  // Dual corners: centroids of triangles on the unit sphere
  std::vector<glm::vec3> dualCorners(triangles.size());
  for (size_t t = 0; t < triangles.size(); ++t) {
    glm::vec3 c = (unitPositions[triangles[t][0]] +
                   unitPositions[triangles[t][1]] +
                   unitPositions[triangles[t][2]]) / 3.0f;
    dualCorners[t] = glm::normalize(c);
  }

  // Map each primal vertex to the triangles containing it
  std::vector<std::vector<uint32_t>> vertexTriangles(unitPositions.size());
  for (size_t t = 0; t < triangles.size(); ++t) {
    vertexTriangles[triangles[t][0]].push_back(static_cast<uint32_t>(t));
    vertexTriangles[triangles[t][1]].push_back(static_cast<uint32_t>(t));
    vertexTriangles[triangles[t][2]].push_back(static_cast<uint32_t>(t));
  }

  hexTiles.resize(unitPositions.size());

  for (size_t v = 0; v < unitPositions.size(); ++v) {
    HexTile &tile = hexTiles[v];
    tile.id = static_cast<uint32_t>(v);
    tile.normal = unitPositions[v];
    tile.center = glm::vec3(this->centerX, this->centerY, this->centerZ) + tile.normal * radius;
    tile.isPentagon = (vertexTriangles[v].size() == 5);

    // Tangent basis on sphere
    glm::vec3 up = (std::abs(tile.normal.y) < 0.99f) ? glm::vec3(0.0f, 1.0f, 0.0f)
                                                     : glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 tangent = glm::normalize(glm::cross(up, tile.normal));
    glm::vec3 bitangent = glm::cross(tile.normal, tangent);

    struct AngleCorner {
      uint32_t triIndex;
      float angle;
    };
    std::vector<AngleCorner> corners;
    for (uint32_t t : vertexTriangles[v]) {
      glm::vec3 d = dualCorners[t] - tile.normal;
      float angle = std::atan2(glm::dot(d, bitangent), glm::dot(d, tangent));
      corners.push_back({t, angle});
    }

    // Sort corners in cyclic order around tile normal
    std::sort(corners.begin(), corners.end(), [](const AngleCorner &a, const AngleCorner &b) {
      return a.angle < b.angle;
    });

    size_t k = corners.size();
    tile.boundaryVertices.resize(k);
    tile.neighbors.resize(k, -1);

    for (size_t i = 0; i < k; ++i) {
      tile.boundaryVertices[i] = dualCorners[corners[i].triIndex];

      uint32_t t1 = corners[i].triIndex;
      uint32_t t2 = corners[(i + 1) % k].triIndex;

      // Find neighbor across boundary edge between t1 and t2
      for (int c1 = 0; c1 < 3; ++c1) {
        uint16_t cand = triangles[t1][c1];
        if (cand == v) continue;
        for (int c2 = 0; c2 < 3; ++c2) {
          if (triangles[t2][c2] == cand) {
            tile.neighbors[i] = static_cast<int32_t>(cand);
            break;
          }
        }
        if (tile.neighbors[i] != -1) break;
      }
    }

    // Assign procedural biomes & colors based on latitude and features
    if (tile.isPentagon) {
      tile.terrainType = 3; // Special Landmark / Volcanic
      tile.color = glm::vec4(0.85f, 0.25f, 0.20f, 1.0f); // Crimson landmark
    } else {
      float lat = std::abs(tile.normal.y);
      float hash = std::sin(static_cast<float>(v) * 12.9898f) * 0.5f + 0.5f;

      if (lat > 0.82f) {
        tile.terrainType = 4; // Polar Ice
        tile.color = glm::vec4(0.92f, 0.95f, 0.98f, 1.0f);
      } else if (lat > 0.65f) {
        tile.terrainType = 5; // Tundra
        tile.color = glm::vec4(0.58f, 0.65f, 0.60f, 1.0f);
      } else if (hash < 0.40f) {
        tile.terrainType = 0; // Ocean
        tile.color = glm::vec4(0.12f, 0.38f, 0.72f, 1.0f);
      } else if (hash < 0.80f) {
        tile.terrainType = 1; // Plains / Forest
        float shade = 0.70f + 0.20f * (static_cast<float>(v % 3) / 2.0f);
        tile.color = glm::vec4(0.22f * shade, 0.65f * shade, 0.28f * shade, 1.0f);
      } else {
        tile.terrainType = 2; // Mountain / Desert
        tile.color = (lat < 0.35f) ? glm::vec4(0.82f, 0.74f, 0.48f, 1.0f)
                                   : glm::vec4(0.55f, 0.50f, 0.45f, 1.0f);
      }
    }

    // Procedural Ore Deposits based on Biomes and Terrain
    tile.structure = TileStructure{};
    tile.deposit = ResourceDeposit{};

    float oreHash = glm::fract(std::sin(static_cast<float>(v) * 43758.5453f + 1.234f));
    float richnessHash = glm::fract(std::sin(static_cast<float>(v) * 23421.631f + 7.89f));

    if (tile.terrainType == 3) {
      // Volcanic Pentagons: 90% chance of Exotic Crystals
      if (oreHash < 0.90f) {
        tile.deposit.type = ResourceType::CRYSTAL;
        tile.deposit.maxReserves = 600 + static_cast<int>(richnessHash * 600.0f);
        tile.deposit.reserves = tile.deposit.maxReserves;
        tile.deposit.baseYield = 16 + static_cast<int>(richnessHash * 14.0f);
      }
    } else if (tile.terrainType == 2) {
      // Mountains & Deserts: 75% Iron, 15% Crystal
      if (oreHash < 0.75f) {
        tile.deposit.type = ResourceType::IRON;
        tile.deposit.maxReserves = 800 + static_cast<int>(richnessHash * 900.0f);
        tile.deposit.reserves = tile.deposit.maxReserves;
        tile.deposit.baseYield = 25 + static_cast<int>(richnessHash * 20.0f);
      } else if (oreHash < 0.90f) {
        tile.deposit.type = ResourceType::CRYSTAL;
        tile.deposit.maxReserves = 500 + static_cast<int>(richnessHash * 400.0f);
        tile.deposit.reserves = tile.deposit.maxReserves;
        tile.deposit.baseYield = 12 + static_cast<int>(richnessHash * 10.0f);
      }
    } else if (tile.terrainType == 4) {
      // Polar Ice: 60% chance of Deuterium Ice
      if (oreHash < 0.60f) {
        tile.deposit.type = ResourceType::DEUTERIUM;
        tile.deposit.maxReserves = 500 + static_cast<int>(richnessHash * 700.0f);
        tile.deposit.reserves = tile.deposit.maxReserves;
        tile.deposit.baseYield = 15 + static_cast<int>(richnessHash * 15.0f);
      }
    } else if (tile.terrainType == 5) {
      // Tundra: 40% Hydrocarbon, 25% Iron
      if (oreHash < 0.40f) {
        tile.deposit.type = ResourceType::HYDROCARBON;
        tile.deposit.maxReserves = 600 + static_cast<int>(richnessHash * 600.0f);
        tile.deposit.reserves = tile.deposit.maxReserves;
        tile.deposit.baseYield = 18 + static_cast<int>(richnessHash * 15.0f);
      } else if (oreHash < 0.65f) {
        tile.deposit.type = ResourceType::IRON;
        tile.deposit.maxReserves = 600 + static_cast<int>(richnessHash * 500.0f);
        tile.deposit.reserves = tile.deposit.maxReserves;
        tile.deposit.baseYield = 18 + static_cast<int>(richnessHash * 12.0f);
      }
    } else if (tile.terrainType == 1) {
      // Plains / Forest: 45% chance of Hydrocarbons / Biomass
      if (oreHash < 0.45f) {
        tile.deposit.type = ResourceType::HYDROCARBON;
        tile.deposit.maxReserves = 700 + static_cast<int>(richnessHash * 800.0f);
        tile.deposit.reserves = tile.deposit.maxReserves;
        tile.deposit.baseYield = 20 + static_cast<int>(richnessHash * 15.0f);
      }
    }

    // Subtle mineral sheen on tiles with active deposits
    if (tile.deposit.type != ResourceType::NONE) {
      glm::vec4 sheen = getResourceColor(tile.deposit.type);
      tile.color = glm::mix(tile.color, sheen, 0.15f);
    }

    // Store base color for resetting highlights
    tile.baseColor = tile.color;
  }

  // Populate vertex and index buffers for rendering
  glm::vec3 centerVec(this->centerX, this->centerY, this->centerZ);
  tileInset = std::clamp(tileInset, 0.5f, 1.0f);

  for (auto &tile : hexTiles) {
    uint16_t centerIdx = static_cast<uint16_t>(Data.size());
    glm::vec3 normal = tile.normal;

    size_t k = tile.boundaryVertices.size();
    tile.vertexStart = static_cast<uint32_t>(Data.size());
    tile.vertexCount = static_cast<uint32_t>(1 + k);

    float uCenter = 0.5f + std::atan2(normal.z, normal.x) / (2.0f * PI);
    float vCenter = 0.5f + std::asin(std::clamp(normal.y, -1.0f, 1.0f)) / PI;
    glm::vec2 uvCenter(uCenter, vCenter);

    Data.push_back({tile.center, normal, tile.color, uvCenter});
    texCoords.push_back(uvCenter);

    uint16_t firstCornerIdx = static_cast<uint16_t>(Data.size());

    for (size_t i = 0; i < k; ++i) {
      glm::vec3 rawCorner = tile.boundaryVertices[i];
      glm::vec3 insetCorner = glm::normalize(tile.normal * (1.0f - tileInset) + rawCorner * tileInset);
      glm::vec3 cornerPos = centerVec + insetCorner * radius;

      float u = 0.5f + std::atan2(insetCorner.z, insetCorner.x) / (2.0f * PI);
      float v = 0.5f + std::asin(std::clamp(insetCorner.y, -1.0f, 1.0f)) / PI;
      glm::vec2 uv(u, v);

      Data.push_back({cornerPos, insetCorner, tile.color, uv});
      texCoords.push_back(uv);
    }

    // Build triangle fan with clockwise winding for Vulkan
    for (size_t i = 0; i < k; ++i) {
      uint16_t idxA = firstCornerIdx + static_cast<uint16_t>(i);
      uint16_t idxB = firstCornerIdx + static_cast<uint16_t>((i + 1) % k);

      glm::vec3 edge1 = Data[idxA].pos - tile.center;
      glm::vec3 edge2 = Data[idxB].pos - tile.center;
      glm::vec3 faceCross = glm::cross(edge1, edge2);

      if (glm::dot(faceCross, normal) < 0.0f) {
        indices.push_back(centerIdx);
        indices.push_back(idxA);
        indices.push_back(idxB);
      } else {
        indices.push_back(centerIdx);
        indices.push_back(idxB);
        indices.push_back(idxA);
      }

      edges.push_back({static_cast<int>(idxA), static_cast<int>(idxB)});
    }
  }
}

int Sphere::findClosestTile(const glm::vec3 &worldPos) const {
  if (hexTiles.empty()) return -1;
  glm::vec3 relPos = worldPos - glm::vec3(centerX, centerY, centerZ);
  float len = glm::length(relPos);
  if (len < 1e-6f) return -1;
  glm::vec3 dir = relPos / len;

  int bestIdx = 0;
  float bestDot = -2.0f;
  for (size_t i = 0; i < hexTiles.size(); ++i) {
    float d = glm::dot(hexTiles[i].normal, dir);
    if (d > bestDot) {
      bestDot = d;
      bestIdx = static_cast<int>(i);
    }
  }
  return bestIdx;
}

void Sphere::setTileColor(int tileId, const glm::vec4 &color, void *mappedGpuBuffer) {
  if (tileId < 0 || tileId >= static_cast<int>(hexTiles.size())) return;
  HexTile &tile = hexTiles[tileId];
  tile.color = color;
  for (uint32_t i = 0; i < tile.vertexCount; ++i) {
    uint32_t idx = tile.vertexStart + i;
    if (idx < Data.size()) {
      Data[idx].colors = color;
      if (mappedGpuBuffer) {
        reinterpret_cast<Vertex *>(mappedGpuBuffer)[idx].colors = color;
      }
    }
  }
}

void Sphere::resetTileColor(int tileId, void *mappedGpuBuffer) {
  if (tileId < 0 || tileId >= static_cast<int>(hexTiles.size())) return;
  setTileColor(tileId, hexTiles[tileId].baseColor, mappedGpuBuffer);
}

std::string Sphere::getTerrainName(int terrainType) {
  switch (terrainType) {
    case 0: return "Ocean";
    case 1: return "Plains / Forest";
    case 2: return "Mountain / Desert";
    case 3: return "Volcanic Crater";
    case 4: return "Polar Ice Cap";
    case 5: return "Tundra";
    default: return "Unknown";
  }
}

std::string Sphere::getResourceName(ResourceType type) {
  switch (type) {
    case ResourceType::IRON: return "Iron Ore";
    case ResourceType::CRYSTAL: return "Exotic Crystal";
    case ResourceType::HYDROCARBON: return "Hydrocarbon Fuel";
    case ResourceType::DEUTERIUM: return "Deuterium Ice";
    default: return "None";
  }
}

std::string Sphere::getStructureName(StructureType type) {
  switch (type) {
    case StructureType::SURFACE_MINE: return "Surface Mine (Tier 1)";
    case StructureType::DEEP_CORE_EXTRACTOR: return "Deep Core Extractor (Tier 2)";
    case StructureType::SOLAR_ARRAY: return "Solar Array";
    default: return "None";
  }
}

glm::vec4 Sphere::getResourceColor(ResourceType type) {
  switch (type) {
    case ResourceType::IRON: return glm::vec4(0.88f, 0.45f, 0.18f, 1.0f); // Bronze / Rust
    case ResourceType::CRYSTAL: return glm::vec4(0.20f, 0.95f, 1.0f, 1.0f); // Neon Cyan
    case ResourceType::HYDROCARBON: return glm::vec4(0.95f, 0.78f, 0.15f, 1.0f); // Amber Gold
    case ResourceType::DEUTERIUM: return glm::vec4(0.40f, 0.98f, 0.85f, 1.0f); // Frosted Teal
    default: return glm::vec4(1.0f);
  }
}


