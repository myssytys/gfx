#include "UnitManager.h"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <iostream>
#include <queue>
#include <stdexcept>

UnitManager::UnitManager() {}

UnitManager::~UnitManager() {
  cleanup();
}

void UnitManager::init(VkDevice dev, VkPhysicalDevice physDev) {
  this->device = dev;
  this->physicalDevice = physDev;
  createBuffers();
}

void UnitManager::cleanup() {
  if (device != VK_NULL_HANDLE) {
    if (vertexBufferMapped) {
      vkUnmapMemory(device, vertexBufferMemory);
      vertexBufferMapped = nullptr;
    }
    if (indexBufferMapped) {
      vkUnmapMemory(device, indexBufferMemory);
      indexBufferMapped = nullptr;
    }
    if (vertexBuffer != VK_NULL_HANDLE) {
      vkDestroyBuffer(device, vertexBuffer, nullptr);
      vertexBuffer = VK_NULL_HANDLE;
    }
    if (vertexBufferMemory != VK_NULL_HANDLE) {
      vkFreeMemory(device, vertexBufferMemory, nullptr);
      vertexBufferMemory = VK_NULL_HANDLE;
    }
    if (indexBuffer != VK_NULL_HANDLE) {
      vkDestroyBuffer(device, indexBuffer, nullptr);
      indexBuffer = VK_NULL_HANDLE;
    }
    if (indexBufferMemory != VK_NULL_HANDLE) {
      vkFreeMemory(device, indexBufferMemory, nullptr);
      indexBufferMemory = VK_NULL_HANDLE;
    }
  }
}

uint32_t UnitManager::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
  VkPhysicalDeviceMemoryProperties memProperties;
  vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

  for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
    if ((typeFilter & (1 << i)) &&
        (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
      return i;
    }
  }
  throw std::runtime_error("failed to find suitable memory type for UnitManager!");
}

void UnitManager::createBuffers() {
  // 1. Vertex Buffer
  VkDeviceSize vertexBufferSize = sizeof(Sphere::Vertex) * MAX_VERTICES;
  VkBufferCreateInfo vbInfo{};
  vbInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  vbInfo.size = vertexBufferSize;
  vbInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
  vbInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateBuffer(device, &vbInfo, nullptr, &vertexBuffer) != VK_SUCCESS) {
    throw std::runtime_error("failed to create unit vertex buffer!");
  }

  VkMemoryRequirements vbMemReqs;
  vkGetBufferMemoryRequirements(device, vertexBuffer, &vbMemReqs);

  VkMemoryAllocateInfo vbAllocInfo{};
  vbAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  vbAllocInfo.allocationSize = vbMemReqs.size;
  vbAllocInfo.memoryTypeIndex = findMemoryType(
      vbMemReqs.memoryTypeBits,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

  if (vkAllocateMemory(device, &vbAllocInfo, nullptr, &vertexBufferMemory) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate unit vertex buffer memory!");
  }
  vkBindBufferMemory(device, vertexBuffer, vertexBufferMemory, 0);
  vkMapMemory(device, vertexBufferMemory, 0, vertexBufferSize, 0, &vertexBufferMapped);

  // 2. Index Buffer
  VkDeviceSize indexBufferSize = sizeof(uint16_t) * MAX_INDICES;
  VkBufferCreateInfo ibInfo{};
  ibInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  ibInfo.size = indexBufferSize;
  ibInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
  ibInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateBuffer(device, &ibInfo, nullptr, &indexBuffer) != VK_SUCCESS) {
    throw std::runtime_error("failed to create unit index buffer!");
  }

  VkMemoryRequirements ibMemReqs;
  vkGetBufferMemoryRequirements(device, indexBuffer, &ibMemReqs);

  VkMemoryAllocateInfo ibAllocInfo{};
  ibAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  ibAllocInfo.allocationSize = ibMemReqs.size;
  ibAllocInfo.memoryTypeIndex = findMemoryType(
      ibMemReqs.memoryTypeBits,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

  if (vkAllocateMemory(device, &ibAllocInfo, nullptr, &indexBufferMemory) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate unit index buffer memory!");
  }
  vkBindBufferMemory(device, indexBuffer, indexBufferMemory, 0);
  vkMapMemory(device, indexBufferMemory, 0, indexBufferSize, 0, &indexBufferMapped);
}

int UnitManager::spawnUnit(UnitType type, int owner, int tileId, const Sphere &planet) {
  if (tileId < 0 || tileId >= static_cast<int>(planet.hexTiles.size())) return -1;

  Unit u{};
  u.id = static_cast<int>(units.size()) + 1;
  u.type = type;
  u.owner = owner;
  u.tileId = tileId;

  const auto &tile = planet.hexTiles[tileId];
  u.currentNormal = tile.normal;
  u.currentPosition = tile.center * 1.03f;

  switch (type) {
    case UnitType::SCOUT:
      u.name = (owner == 0) ? "Scout" : "Alien Scout";
      u.movementPoints = 3;
      u.maxMovementPoints = 3;
      u.health = 10;
      u.maxHealth = 10;
      u.color = (owner == 0) ? glm::vec4(0.2f, 0.95f, 1.0f, 1.0f) : glm::vec4(1.0f, 0.25f, 0.25f, 1.0f);
      break;

    case UnitType::WARRIOR:
      u.name = (owner == 0) ? "Warrior" : "Alien Warrior";
      u.movementPoints = 2;
      u.maxMovementPoints = 2;
      u.health = 20;
      u.maxHealth = 20;
      u.color = (owner == 0) ? glm::vec4(1.0f, 0.75f, 0.15f, 1.0f) : glm::vec4(1.0f, 0.35f, 0.15f, 1.0f);
      break;

    case UnitType::SETTLER:
      u.name = "Settler";
      u.movementPoints = 2;
      u.maxMovementPoints = 2;
      u.health = 10;
      u.maxHealth = 10;
      u.color = glm::vec4(0.35f, 1.0f, 0.45f, 1.0f);
      break;
  }

  units.push_back(u);
  return u.id;
}

void UnitManager::spawnDefaultUnits(const Sphere &planet) {
  units.clear();
  selectedUnitId = -1;

  // Find candidate land tiles (Plains = 1, Tundra = 5)
  std::vector<int> landTiles;
  for (size_t i = 0; i < planet.hexTiles.size(); ++i) {
    if (planet.hexTiles[i].terrainType == 1 || planet.hexTiles[i].terrainType == 5) {
      landTiles.push_back(static_cast<int>(i));
    }
  }

  if (landTiles.empty()) {
    std::cerr << "[UnitManager] Warning: No land tiles found for unit spawning!\n";
    return;
  }

  // 1. Pick a base land tile for player Scout
  int playerBaseTile = landTiles[landTiles.size() / 4];
  spawnUnit(UnitType::SCOUT, 0, playerBaseTile, planet);

  // 2. Spawn player Warrior on an adjacent land neighbor if possible
  int warriorTile = -1;
  for (int n : planet.hexTiles[playerBaseTile].neighbors) {
    if (n >= 0 && n < static_cast<int>(planet.hexTiles.size())) {
      if (planet.hexTiles[n].terrainType == 1 || planet.hexTiles[n].terrainType == 5) {
        warriorTile = n;
        break;
      }
    }
  }
  if (warriorTile == -1) warriorTile = landTiles[(landTiles.size() / 4) + 1];
  spawnUnit(UnitType::WARRIOR, 0, warriorTile, planet);

  // 3. Spawn Settler nearby
  int settlerTile = -1;
  for (int n : planet.hexTiles[warriorTile].neighbors) {
    if (n >= 0 && n < static_cast<int>(planet.hexTiles.size()) && n != playerBaseTile) {
      if (planet.hexTiles[n].terrainType == 1 || planet.hexTiles[n].terrainType == 5) {
        settlerTile = n;
        break;
      }
    }
  }
  if (settlerTile == -1) settlerTile = landTiles[(landTiles.size() / 4) + 2];
  spawnUnit(UnitType::SETTLER, 0, settlerTile, planet);

  // 4. Spawn Rival Scout on opposite hemisphere
  int rivalTile = landTiles[3 * landTiles.size() / 4];
  spawnUnit(UnitType::SCOUT, 1, rivalTile, planet);

  // Select the player's scout by default
  selectedUnitId = 1;

  std::cout << "[UnitManager] Spawned 3 Player Units (Scout #" << units[0].id
            << " at Tile " << playerBaseTile << ", Warrior #" << units[1].id
            << " at Tile " << warriorTile << ", Settler #" << units[2].id
            << " at Tile " << settlerTile << ") and 1 Rival Scout at Tile "
            << rivalTile << ".\n";
}

Unit *UnitManager::getUnit(int unitId) {
  for (auto &u : units) {
    if (u.id == unitId) return &u;
  }
  return nullptr;
}

const Unit *UnitManager::getUnit(int unitId) const {
  for (const auto &u : units) {
    if (u.id == unitId) return &u;
  }
  return nullptr;
}

Unit *UnitManager::getUnitAtTile(int tileId) {
  for (auto &u : units) {
    if (u.tileId == tileId) return &u;
  }
  return nullptr;
}

const Unit *UnitManager::getUnitAtTile(int tileId) const {
  for (const auto &u : units) {
    if (u.tileId == tileId) return &u;
  }
  return nullptr;
}

Unit *UnitManager::getSelectedUnit() {
  return getUnit(selectedUnitId);
}

void UnitManager::selectUnit(int unitId) {
  selectedUnitId = unitId;
}

void UnitManager::deselectUnit() {
  selectedUnitId = -1;
}

int UnitManager::selectNextPlayerUnit() {
  std::vector<int> playerUnitIds;
  for (const auto &u : units) {
    if (u.owner == 0) {
      playerUnitIds.push_back(u.id);
    }
  }
  if (playerUnitIds.empty()) return -1;

  int nextIndex = 0;
  for (size_t i = 0; i < playerUnitIds.size(); ++i) {
    if (playerUnitIds[i] == selectedUnitId) {
      nextIndex = (i + 1) % playerUnitIds.size();
      break;
    }
  }

  selectedUnitId = playerUnitIds[nextIndex];
  return selectedUnitId;
}

void UnitManager::nextTurn() {
  currentTurn++;
  for (auto &u : units) {
    u.movementPoints = u.maxMovementPoints;
  }
  std::cout << "\n=========================================\n";
  std::cout << ">>> TURN " << currentTurn << " STARTED! <<<\n";
  std::cout << "  All player and faction units recharged full Movement Points.\n";
  std::cout << "=========================================\n" << std::endl;
}

bool UnitManager::isTilePassable(int tileId, const Sphere &planet, const Unit &unit) const {
  if (tileId < 0 || tileId >= static_cast<int>(planet.hexTiles.size())) return false;
  const auto &tile = planet.hexTiles[tileId];

  // Ocean is impassable for land units
  if (tile.terrainType == 0) return false;

  // Settlers cannot cross rugged mountains or polar ice
  if (unit.type == UnitType::SETTLER) {
    if (tile.terrainType == 2 || tile.terrainType == 4) return false;
  }

  return true;
}

int UnitManager::getTileMovementCost(int tileId, const Sphere &planet, const Unit &unit) const {
  if (tileId < 0 || tileId >= static_cast<int>(planet.hexTiles.size())) return 999;
  const auto &tile = planet.hexTiles[tileId];

  switch (tile.terrainType) {
    case 0: return 999; // Ocean (impassable)
    case 1: return 1;   // Plains
    case 5: return 1;   // Tundra
    case 2: return 2;   // Mountain
    case 3: return 2;   // Volcanic Landmark
    case 4: return 2;   // Polar Ice
    default: return 1;
  }
}

std::vector<int> UnitManager::getReachableTiles(int unitId, const Sphere &planet) const {
  const Unit *u = getUnit(unitId);
  if (!u || u->movementPoints <= 0 || u->isMoving) return {};

  std::unordered_map<int, int> dist;
  std::priority_queue<std::pair<int, int>,
                      std::vector<std::pair<int, int>>,
                      std::greater<std::pair<int, int>>> pq;

  dist[u->tileId] = 0;
  pq.push({0, u->tileId});

  while (!pq.empty()) {
    auto [currentCost, tileId] = pq.top();
    pq.pop();

    if (currentCost > dist[tileId]) continue;

    const auto &tile = planet.hexTiles[tileId];
    for (int neighborId : tile.neighbors) {
      if (neighborId < 0 || neighborId >= static_cast<int>(planet.hexTiles.size())) continue;
      if (!isTilePassable(neighborId, planet, *u)) continue;

      int cost = getTileMovementCost(neighborId, planet, *u);
      int newCost = currentCost + cost;

      if (newCost <= u->movementPoints) {
        if (dist.find(neighborId) == dist.end() || newCost < dist[neighborId]) {
          dist[neighborId] = newCost;
          pq.push({newCost, neighborId});
        }
      }
    }
  }

  std::vector<int> reachable;
  for (const auto &pair : dist) {
    if (pair.first != u->tileId) {
      // 1-Unit-Per-Tile check: cannot finish move on another unit
      bool occupied = false;
      for (const auto &other : units) {
        if (other.id != u->id && other.tileId == pair.first) {
          occupied = true;
          break;
        }
      }
      if (!occupied) {
        reachable.push_back(pair.first);
      }
    }
  }

  return reachable;
}

std::vector<int> UnitManager::findPath(int unitId, int targetTileId, const Sphere &planet) const {
  const Unit *u = getUnit(unitId);
  if (!u || u->movementPoints <= 0 || u->isMoving) return {};
  if (targetTileId == u->tileId) return {};

  std::unordered_map<int, int> dist;
  std::unordered_map<int, int> parent;
  std::priority_queue<std::pair<int, int>,
                      std::vector<std::pair<int, int>>,
                      std::greater<std::pair<int, int>>> pq;

  dist[u->tileId] = 0;
  pq.push({0, u->tileId});

  while (!pq.empty()) {
    auto [currentCost, tileId] = pq.top();
    pq.pop();

    if (tileId == targetTileId) break;
    if (currentCost > dist[tileId]) continue;

    const auto &tile = planet.hexTiles[tileId];
    for (int neighborId : tile.neighbors) {
      if (neighborId < 0 || neighborId >= static_cast<int>(planet.hexTiles.size())) continue;
      if (!isTilePassable(neighborId, planet, *u)) continue;

      int cost = getTileMovementCost(neighborId, planet, *u);
      int newCost = currentCost + cost;

      if (newCost <= u->movementPoints) {
        if (dist.find(neighborId) == dist.end() || newCost < dist[neighborId]) {
          dist[neighborId] = newCost;
          parent[neighborId] = tileId;
          pq.push({newCost, neighborId});
        }
      }
    }
  }

  if (dist.find(targetTileId) == dist.end()) return {};

  std::vector<int> path;
  int curr = targetTileId;
  while (curr != u->tileId) {
    path.push_back(curr);
    curr = parent[curr];
  }
  path.push_back(u->tileId);
  std::reverse(path.begin(), path.end());

  return path;
}

bool UnitManager::moveUnit(int unitId, int targetTileId, const Sphere &planet) {
  Unit *u = getUnit(unitId);
  if (!u || u->isMoving) return false;

  auto path = findPath(unitId, targetTileId, planet);
  if (path.size() < 2) return false;

  // Calculate total path cost
  int totalCost = 0;
  for (size_t i = 1; i < path.size(); ++i) {
    totalCost += getTileMovementCost(path[i], planet, *u);
  }

  if (totalCost > u->movementPoints) return false;

  u->movementPoints -= totalCost;
  u->path = path;
  u->currentPathIdx = 0;
  u->moveProgress = 0.0f;
  u->isMoving = true;

  std::cout << "[UnitManager] " << u->name << " #" << u->id << " moving from Tile #"
            << path.front() << " to Tile #" << path.back() << " (" << path.size() - 1
            << " steps, cost: " << totalCost << " MP, remaining: " << u->movementPoints << " MP)\n";

  return true;
}

void UnitManager::update(float deltaTime, float totalTime, const Sphere &planet) {
  // Update unit moving animations
  const float MOVE_SPEED = 3.5f; // Steps per second

  for (auto &u : units) {
    if (u.isMoving && u.path.size() >= 2) {
      u.moveProgress += deltaTime * MOVE_SPEED;

      if (u.moveProgress >= 1.0f) {
        u.moveProgress = 0.0f;
        u.currentPathIdx++;

        if (u.currentPathIdx + 1 >= u.path.size()) {
          // Reached destination tile
          u.tileId = u.path.back();
          u.isMoving = false;
          u.path.clear();
          u.currentPathIdx = 0;

          const auto &tile = planet.hexTiles[u.tileId];
          u.currentNormal = tile.normal;
          u.currentPosition = tile.center * 1.03f;
          continue;
        }
      }

      int fromTile = u.path[u.currentPathIdx];
      int toTile = u.path[u.currentPathIdx + 1];

      const auto &tileA = planet.hexTiles[fromTile];
      const auto &tileB = planet.hexTiles[toTile];

      // Spherical interpolation along planetary surface
      glm::vec3 dir = glm::normalize(glm::mix(tileA.center, tileB.center, u.moveProgress));
      float hop = std::sin(u.moveProgress * 3.14159f) * 0.035f;
      u.currentNormal = dir;
      u.currentPosition = dir * (1.03f + hop);
    } else {
      // Idle on its tile
      if (u.tileId >= 0 && u.tileId < static_cast<int>(planet.hexTiles.size())) {
        const auto &tile = planet.hexTiles[u.tileId];
        u.currentNormal = tile.normal;
        u.currentPosition = tile.center * 1.03f;
      }
    }
  }

  // Rebuild unit 3D geometry and copy to mapped GPU buffers
  buildUnitMeshes(totalTime, planet);
}

void UnitManager::buildUnitMeshes(float totalTime, const Sphere &planet) {
  if (!vertexBufferMapped || !indexBufferMapped) return;

  std::vector<Sphere::Vertex> verts;
  std::vector<uint16_t> indices;
  verts.reserve(2048);
  indices.reserve(4096);

  // 1. Render Natural Ore Deposits on unbuilt tiles
  for (const auto &tile : planet.hexTiles) {
    if (tile.deposit.type != Sphere::ResourceType::NONE &&
        tile.structure.type == Sphere::StructureType::NONE) {
      addOreDepositMesh(tile, totalTime, verts, indices);
    }
  }

  // 2. Render Constructed Structures (Mines, Deep Core Rigs, Solar Plants)
  for (const auto &tile : planet.hexTiles) {
    if (tile.structure.type == Sphere::StructureType::SURFACE_MINE) {
      addMineMesh(tile, totalTime, verts, indices);
    } else if (tile.structure.type == Sphere::StructureType::DEEP_CORE_EXTRACTOR) {
      addDeepCoreMesh(tile, totalTime, verts, indices);
    } else if (tile.structure.type == Sphere::StructureType::SOLAR_ARRAY) {
      addSolarArrayMesh(tile, totalTime, verts, indices);
    }
  }

  // 3. Render Units
  for (const auto &u : units) {
    bool isSelected = (u.id == selectedUnitId);

    // Subtle bobbing height
    float bob = std::sin(totalTime * 3.5f + static_cast<float>(u.id) * 1.5f) * 0.008f;
    glm::vec3 pos = u.currentPosition + u.currentNormal * bob;

    switch (u.type) {
      case UnitType::SCOUT:
        addScoutMesh(u, pos, u.currentNormal, totalTime, isSelected, verts, indices);
        break;
      case UnitType::WARRIOR:
        addWarriorMesh(u, pos, u.currentNormal, totalTime, isSelected, verts, indices);
        break;
      case UnitType::SETTLER:
        addSettlerMesh(u, pos, u.currentNormal, totalTime, isSelected, verts, indices);
        break;
    }

    if (isSelected) {
      addSelectionRing(pos, u.currentNormal, totalTime, verts, indices);
    }
  }

  indexCount = static_cast<uint32_t>(indices.size());
  if (indexCount > 0 && verts.size() < MAX_VERTICES && indices.size() < MAX_INDICES) {
    memcpy(vertexBufferMapped, verts.data(), verts.size() * sizeof(Sphere::Vertex));
    memcpy(indexBufferMapped, indices.data(), indices.size() * sizeof(uint16_t));
  }
}

void UnitManager::addScoutMesh(const Unit &unit, const glm::vec3 &pos, const glm::vec3 &normal,
                               float totalTime, bool isSelected,
                               std::vector<Sphere::Vertex> &verts, std::vector<uint16_t> &indices) {
  // Construct orthonormal tangent frame
  glm::vec3 up = normal;
  glm::vec3 temp = (std::abs(up.y) > 0.9f) ? glm::vec3(1.0f, 0.0f, 0.0f) : glm::vec3(0.0f, 1.0f, 0.0f);
  glm::vec3 tan = glm::normalize(glm::cross(up, temp));
  glm::vec3 bitan = glm::normalize(glm::cross(up, tan));

  float rotSpeed = isSelected ? 3.0f : 1.5f;
  float angle = totalTime * rotSpeed + static_cast<float>(unit.id);
  glm::vec3 rT = tan * std::cos(angle) + bitan * std::sin(angle);
  glm::vec3 rB = -tan * std::sin(angle) + bitan * std::cos(angle);

  // Twin pyramid diamond mesh
  glm::vec3 topApex = pos + up * 0.065f;
  glm::vec3 btmApex = pos - up * 0.020f;
  float rad = 0.032f;
  glm::vec3 c1 = pos + rT * rad;
  glm::vec3 c2 = pos + rB * rad;
  glm::vec3 c3 = pos - rT * rad;
  glm::vec3 c4 = pos - rB * rad;

  glm::vec4 color = unit.color;
  if (isSelected) {
    color = glm::mix(color, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 0.35f);
  }

  uint16_t base = static_cast<uint16_t>(verts.size());

  // 6 vertices
  verts.push_back({topApex, up, color, {0.5f, 0.5f}});
  verts.push_back({btmApex, -up, color * 0.8f, {0.5f, 0.5f}});
  verts.push_back({c1, rT, color, {0.5f, 0.5f}});
  verts.push_back({c2, rB, color, {0.5f, 0.5f}});
  verts.push_back({c3, -rT, color, {0.5f, 0.5f}});
  verts.push_back({c4, -rB, color, {0.5f, 0.5f}});

  // Top faces: (top, c1, c2), (top, c2, c3), (top, c3, c4), (top, c4, c1)
  indices.push_back(base + 0); indices.push_back(base + 2); indices.push_back(base + 3);
  indices.push_back(base + 0); indices.push_back(base + 3); indices.push_back(base + 4);
  indices.push_back(base + 0); indices.push_back(base + 4); indices.push_back(base + 5);
  indices.push_back(base + 0); indices.push_back(base + 5); indices.push_back(base + 2);

  // Bottom faces: (btm, c2, c1), (btm, c3, c2), (btm, c4, c3), (btm, c1, c4)
  indices.push_back(base + 1); indices.push_back(base + 3); indices.push_back(base + 2);
  indices.push_back(base + 1); indices.push_back(base + 4); indices.push_back(base + 3);
  indices.push_back(base + 1); indices.push_back(base + 5); indices.push_back(base + 4);
  indices.push_back(base + 1); indices.push_back(base + 2); indices.push_back(base + 5);
}

void UnitManager::addWarriorMesh(const Unit &unit, const glm::vec3 &pos, const glm::vec3 &normal,
                                 float totalTime, bool isSelected,
                                 std::vector<Sphere::Vertex> &verts, std::vector<uint16_t> &indices) {
  glm::vec3 up = normal;
  glm::vec3 temp = (std::abs(up.y) > 0.9f) ? glm::vec3(1.0f, 0.0f, 0.0f) : glm::vec3(0.0f, 1.0f, 0.0f);
  glm::vec3 tan = glm::normalize(glm::cross(up, temp));
  glm::vec3 bitan = glm::normalize(glm::cross(up, tan));

  float rotSpeed = isSelected ? 2.5f : 1.2f;
  float angle = totalTime * rotSpeed + static_cast<float>(unit.id);
  glm::vec3 rT = tan * std::cos(angle) + bitan * std::sin(angle);
  glm::vec3 rB = -tan * std::sin(angle) + bitan * std::cos(angle);

  // Robust Octahedron crystal with protective chevron crest
  glm::vec3 topApex = pos + up * 0.075f;
  glm::vec3 btmApex = pos - up * 0.025f;
  float rad = 0.042f;
  glm::vec3 c1 = pos + rT * rad;
  glm::vec3 c2 = pos + rB * rad;
  glm::vec3 c3 = pos - rT * rad;
  glm::vec3 c4 = pos - rB * rad;

  glm::vec4 color = unit.color;
  if (isSelected) {
    color = glm::mix(color, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 0.35f);
  }

  uint16_t base = static_cast<uint16_t>(verts.size());

  verts.push_back({topApex, up, color * 1.15f, {0.5f, 0.5f}});
  verts.push_back({btmApex, -up, color * 0.75f, {0.5f, 0.5f}});
  verts.push_back({c1, rT, color, {0.5f, 0.5f}});
  verts.push_back({c2, rB, color, {0.5f, 0.5f}});
  verts.push_back({c3, -rT, color, {0.5f, 0.5f}});
  verts.push_back({c4, -rB, color, {0.5f, 0.5f}});

  // Top faces
  indices.push_back(base + 0); indices.push_back(base + 2); indices.push_back(base + 3);
  indices.push_back(base + 0); indices.push_back(base + 3); indices.push_back(base + 4);
  indices.push_back(base + 0); indices.push_back(base + 4); indices.push_back(base + 5);
  indices.push_back(base + 0); indices.push_back(base + 5); indices.push_back(base + 2);

  // Bottom faces
  indices.push_back(base + 1); indices.push_back(base + 3); indices.push_back(base + 2);
  indices.push_back(base + 1); indices.push_back(base + 4); indices.push_back(base + 3);
  indices.push_back(base + 1); indices.push_back(base + 5); indices.push_back(base + 4);
  indices.push_back(base + 1); indices.push_back(base + 2); indices.push_back(base + 5);
}

void UnitManager::addSettlerMesh(const Unit &unit, const glm::vec3 &pos, const glm::vec3 &normal,
                                 float totalTime, bool isSelected,
                                 std::vector<Sphere::Vertex> &verts, std::vector<uint16_t> &indices) {
  glm::vec3 up = normal;
  glm::vec3 temp = (std::abs(up.y) > 0.9f) ? glm::vec3(1.0f, 0.0f, 0.0f) : glm::vec3(0.0f, 1.0f, 0.0f);
  glm::vec3 tan = glm::normalize(glm::cross(up, temp));
  glm::vec3 bitan = glm::normalize(glm::cross(up, tan));

  float rotSpeed = isSelected ? 2.0f : 1.0f;
  float angle = totalTime * rotSpeed + static_cast<float>(unit.id);
  glm::vec3 rT = tan * std::cos(angle) + bitan * std::sin(angle);
  glm::vec3 rB = -tan * std::sin(angle) + bitan * std::cos(angle);

  // Hexagonal citadel dome with apex spire
  glm::vec3 spire = pos + up * 0.065f;
  glm::vec3 baseCenter = pos - up * 0.015f;
  float rad = 0.038f;

  uint16_t base = static_cast<uint16_t>(verts.size());
  glm::vec4 color = unit.color;

  verts.push_back({spire, up, color * 1.2f, {0.5f, 0.5f}});      // Index 0: Apex
  verts.push_back({baseCenter, -up, color * 0.7f, {0.5f, 0.5f}}); // Index 1: Bottom center

  const int SIDES = 6;
  for (int i = 0; i < SIDES; ++i) {
    float a = static_cast<float>(i) * (6.2831853f / SIDES);
    glm::vec3 rim = pos + (rT * std::cos(a) + rB * std::sin(a)) * rad;
    verts.push_back({rim, glm::normalize(rim - pos), color, {0.5f, 0.5f}});
  }

  // Roof pyramid faces
  for (int i = 0; i < SIDES; ++i) {
    uint16_t curr = static_cast<uint16_t>(base + 2 + i);
    uint16_t next = static_cast<uint16_t>(base + 2 + ((i + 1) % SIDES));

    indices.push_back(base + 0);
    indices.push_back(curr);
    indices.push_back(next);

    // Floor base faces
    indices.push_back(base + 1);
    indices.push_back(next);
    indices.push_back(curr);
  }
}

void UnitManager::addSelectionRing(const glm::vec3 &pos, const glm::vec3 &normal,
                                   float totalTime,
                                   std::vector<Sphere::Vertex> &verts, std::vector<uint16_t> &indices) {
  glm::vec3 up = normal;
  glm::vec3 temp = (std::abs(up.y) > 0.9f) ? glm::vec3(1.0f, 0.0f, 0.0f) : glm::vec3(0.0f, 1.0f, 0.0f);
  glm::vec3 tan = glm::normalize(glm::cross(up, temp));
  glm::vec3 bitan = glm::normalize(glm::cross(up, tan));

  // Hovering golden halo above unit
  float pulse = std::sin(totalTime * 6.0f) * 0.005f;
  float ringRadInner = 0.045f + pulse;
  float ringRadOuter = 0.058f + pulse;
  glm::vec3 ringPos = pos + up * 0.080f;

  glm::vec4 haloColor(1.0f, 0.90f, 0.25f, 1.0f);
  uint16_t base = static_cast<uint16_t>(verts.size());
  const int SEGMENTS = 12;

  for (int i = 0; i < SEGMENTS; ++i) {
    float a = static_cast<float>(i) * (6.2831853f / SEGMENTS);
    glm::vec3 dir = tan * std::cos(a) + bitan * std::sin(a);
    verts.push_back({ringPos + dir * ringRadInner, up, haloColor, {0.5f, 0.5f}});
    verts.push_back({ringPos + dir * ringRadOuter, up, haloColor * 0.7f, {0.5f, 0.5f}});
  }

  for (int i = 0; i < SEGMENTS; ++i) {
    int next = (i + 1) % SEGMENTS;
    uint16_t i0 = static_cast<uint16_t>(base + i * 2);
    uint16_t o0 = static_cast<uint16_t>(base + i * 2 + 1);
    uint16_t i1 = static_cast<uint16_t>(base + next * 2);
    uint16_t o1 = static_cast<uint16_t>(base + next * 2 + 1);

    indices.push_back(i0); indices.push_back(o0); indices.push_back(i1);
    indices.push_back(i1); indices.push_back(o0); indices.push_back(o1);
  }
}

void UnitManager::recordRender(VkCommandBuffer commandBuffer) {
  if (indexCount > 0 && vertexBuffer != VK_NULL_HANDLE && indexBuffer != VK_NULL_HANDLE) {
    VkBuffer vbs[] = {vertexBuffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vbs, offsets);
    vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT16);
    vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
  }
}

void UnitManager::addOreDepositMesh(const Sphere::HexTile &tile, float totalTime,
                                    std::vector<Sphere::Vertex> &verts, std::vector<uint16_t> &indices) {
  if (verts.size() + 20 >= MAX_VERTICES || indices.size() + 40 >= MAX_INDICES) return;

  glm::vec3 up = tile.normal;
  glm::vec3 temp = (std::abs(up.y) > 0.9f) ? glm::vec3(1.0f, 0.0f, 0.0f) : glm::vec3(0.0f, 1.0f, 0.0f);
  glm::vec3 tan = glm::normalize(glm::cross(up, temp));
  glm::vec3 bitan = glm::normalize(glm::cross(up, tan));

  glm::vec3 pos = tile.center * 1.008f;
  glm::vec4 color = Sphere::getResourceColor(tile.deposit.type);
  float shimmer = 1.0f + 0.18f * std::sin(totalTime * 3.0f + static_cast<float>(tile.id));
  color = glm::clamp(color * shimmer, 0.0f, 1.0f);

  // 3 faceted crystalline shards / ore nuggets
  float angles[3] = {0.0f, 2.1f, 4.2f};
  float dists[3] = {0.020f, 0.024f, 0.018f};
  float heights[3] = {0.035f, 0.048f, 0.028f};
  float radii[3] = {0.010f, 0.012f, 0.009f};

  for (int s = 0; s < 3; ++s) {
    if (verts.size() + 6 >= MAX_VERTICES || indices.size() + 12 >= MAX_INDICES) return;
    glm::vec3 cPos = pos + (tan * std::cos(angles[s]) + bitan * std::sin(angles[s])) * dists[s];
    glm::vec3 tip = cPos + up * heights[s];
    uint16_t base = static_cast<uint16_t>(verts.size());

    verts.push_back({tip, up, color * 1.3f, {0.5f, 0.5f}}); // 0: tip

    // 4 base corners
    for (int c = 0; c < 4; ++c) {
      float ca = static_cast<float>(c) * 1.5707963f;
      glm::vec3 bCorner = cPos + (tan * std::cos(ca) + bitan * std::sin(ca)) * radii[s];
      verts.push_back({bCorner, glm::normalize(bCorner - cPos), color * 0.85f, {0.5f, 0.5f}});
    }

    // 4 triangular sides
    for (int c = 0; c < 4; ++c) {
      uint16_t i0 = static_cast<uint16_t>(base + 1 + c);
      uint16_t i1 = static_cast<uint16_t>(base + 1 + ((c + 1) % 4));
      indices.push_back(base);
      indices.push_back(i0);
      indices.push_back(i1);
    }
  }
}

void UnitManager::addMineMesh(const Sphere::HexTile &tile, float totalTime,
                              std::vector<Sphere::Vertex> &verts, std::vector<uint16_t> &indices) {
  if (verts.size() + 40 >= MAX_VERTICES || indices.size() + 80 >= MAX_INDICES) return;

  glm::vec3 up = tile.normal;
  glm::vec3 temp = (std::abs(up.y) > 0.9f) ? glm::vec3(1.0f, 0.0f, 0.0f) : glm::vec3(0.0f, 1.0f, 0.0f);
  glm::vec3 tan = glm::normalize(glm::cross(up, temp));
  glm::vec3 bitan = glm::normalize(glm::cross(up, tan));

  glm::vec3 pos = tile.center * 1.01f;
  glm::vec4 frameColor(0.82f, 0.84f, 0.88f, 1.0f); // Industrial Steel
  glm::vec4 resColor = Sphere::getResourceColor(tile.deposit.type);

  float derrickHeight = 0.065f;
  float baseRadius = 0.042f;
  float topRadius = 0.016f;

  uint16_t baseIdx = static_cast<uint16_t>(verts.size());

  // 4 Bottom Feet
  for (int i = 0; i < 4; ++i) {
    float a = static_cast<float>(i) * 1.5707963f + 0.785398f;
    glm::vec3 foot = pos + (tan * std::cos(a) + bitan * std::sin(a)) * baseRadius;
    verts.push_back({foot, up, frameColor * 0.7f, {0.5f, 0.5f}});
  }

  // 4 Top Platform Corners
  glm::vec3 crownPos = pos + up * derrickHeight;
  for (int i = 0; i < 4; ++i) {
    float a = static_cast<float>(i) * 1.5707963f + 0.785398f;
    glm::vec3 topC = crownPos + (tan * std::cos(a) + bitan * std::sin(a)) * topRadius;
    verts.push_back({topC, up, frameColor, {0.5f, 0.5f}});
  }

  // Derrick frame sides
  for (int i = 0; i < 4; ++i) {
    int next = (i + 1) % 4;
    uint16_t b0 = static_cast<uint16_t>(baseIdx + i);
    uint16_t b1 = static_cast<uint16_t>(baseIdx + next);
    uint16_t t0 = static_cast<uint16_t>(baseIdx + 4 + i);
    uint16_t t1 = static_cast<uint16_t>(baseIdx + 4 + next);

    indices.push_back(b0); indices.push_back(t0); indices.push_back(b1);
    indices.push_back(b1); indices.push_back(t0); indices.push_back(t1);
  }

  // Crown top plate
  uint16_t t0 = static_cast<uint16_t>(baseIdx + 4);
  uint16_t t1 = static_cast<uint16_t>(baseIdx + 5);
  uint16_t t2 = static_cast<uint16_t>(baseIdx + 6);
  uint16_t t3 = static_cast<uint16_t>(baseIdx + 7);
  indices.push_back(t0); indices.push_back(t1); indices.push_back(t2);
  indices.push_back(t0); indices.push_back(t2); indices.push_back(t3);

  // Animated Drilling Piston / Shaft in the center
  float stroke = (std::sin(totalTime * 8.0f) * 0.5f + 0.5f) * 0.025f;
  glm::vec3 drillTip = pos + up * (0.015f + stroke);
  glm::vec3 drillTop = pos + up * (derrickHeight + 0.015f);

  uint16_t drillBase = static_cast<uint16_t>(verts.size());
  verts.push_back({drillTop, up, resColor * 1.2f, {0.5f, 0.5f}});
  verts.push_back({drillTip, -up, resColor * 1.4f, {0.5f, 0.5f}});

  for (int i = 0; i < 4; ++i) {
    float a = static_cast<float>(i) * 1.5707963f;
    glm::vec3 p = drillTip + up * 0.02f + (tan * std::cos(a) + bitan * std::sin(a)) * 0.007f;
    verts.push_back({p, up, resColor, {0.5f, 0.5f}});
  }

  for (int i = 0; i < 4; ++i) {
    int next = (i + 1) % 4;
    uint16_t c0 = static_cast<uint16_t>(drillBase + 2 + i);
    uint16_t c1 = static_cast<uint16_t>(drillBase + 2 + next);
    indices.push_back(drillBase); indices.push_back(c0); indices.push_back(c1);
    indices.push_back(drillBase + 1); indices.push_back(c1); indices.push_back(c0);
  }
}

void UnitManager::addDeepCoreMesh(const Sphere::HexTile &tile, float totalTime,
                                  std::vector<Sphere::Vertex> &verts, std::vector<uint16_t> &indices) {
  if (verts.size() + 50 >= MAX_VERTICES || indices.size() + 100 >= MAX_INDICES) return;

  glm::vec3 up = tile.normal;
  glm::vec3 temp = (std::abs(up.y) > 0.9f) ? glm::vec3(1.0f, 0.0f, 0.0f) : glm::vec3(0.0f, 1.0f, 0.0f);
  glm::vec3 tan = glm::normalize(glm::cross(up, temp));
  glm::vec3 bitan = glm::normalize(glm::cross(up, tan));

  glm::vec3 pos = tile.center * 1.01f;
  glm::vec4 darkArmor(0.22f, 0.24f, 0.28f, 1.0f);
  glm::vec4 glowColor = Sphere::getResourceColor(tile.deposit.type) * 1.5f;

  float towerHeight = 0.085f;
  float pylonDist = 0.038f;
  float pylonRad = 0.010f;

  // Dual Heavy Pylons on left and right
  for (int side = -1; side <= 1; side += 2) {
    if (verts.size() + 6 >= MAX_VERTICES || indices.size() + 18 >= MAX_INDICES) return;
    glm::vec3 pBase = pos + tan * (static_cast<float>(side) * pylonDist);
    glm::vec3 pTop = pBase + up * towerHeight;
    uint16_t pIdx = static_cast<uint16_t>(verts.size());

    verts.push_back({pTop, up, darkArmor * 1.4f, {0.5f, 0.5f}});
    verts.push_back({pBase, -up, darkArmor * 0.7f, {0.5f, 0.5f}});

    for (int i = 0; i < 4; ++i) {
      float a = static_cast<float>(i) * 1.5707963f;
      glm::vec3 c = pBase + up * (towerHeight * 0.5f) + (tan * std::cos(a) + bitan * std::sin(a)) * pylonRad;
      verts.push_back({c, up, darkArmor, {0.5f, 0.5f}});
    }

    for (int i = 0; i < 4; ++i) {
      int next = (i + 1) % 4;
      uint16_t c0 = static_cast<uint16_t>(pIdx + 2 + i);
      uint16_t c1 = static_cast<uint16_t>(pIdx + 2 + next);
      indices.push_back(pIdx); indices.push_back(c0); indices.push_back(c1);
      indices.push_back(pIdx + 1); indices.push_back(c1); indices.push_back(c0);
    }
  }

  // Rotating Central Plasma Ring
  float rot = totalTime * 4.5f;
  glm::vec3 ringCenter = pos + up * 0.055f;
  float ringR = 0.026f;
  const int SEGS = 8;
  uint16_t ringBase = static_cast<uint16_t>(verts.size());

  for (int i = 0; i < SEGS; ++i) {
    float a = static_cast<float>(i) * (6.2831853f / SEGS) + rot;
    glm::vec3 rPos = ringCenter + (tan * std::cos(a) + bitan * std::sin(a)) * ringR;
    verts.push_back({rPos, up, glowColor, {0.5f, 0.5f}});
  }

  for (int i = 0; i < SEGS; ++i) {
    int next = (i + 1) % SEGS;
    uint16_t i0 = static_cast<uint16_t>(ringBase + i);
    uint16_t i1 = static_cast<uint16_t>(ringBase + next);
    uint16_t coreIdx = static_cast<uint16_t>(verts.size());
    verts.push_back({ringCenter, up, glowColor * 1.8f, {0.5f, 0.5f}});
    indices.push_back(coreIdx); indices.push_back(i0); indices.push_back(i1);
  }
}

void UnitManager::addSolarArrayMesh(const Sphere::HexTile &tile, float totalTime,
                                    std::vector<Sphere::Vertex> &verts, std::vector<uint16_t> &indices) {
  if (verts.size() + 30 >= MAX_VERTICES || indices.size() + 50 >= MAX_INDICES) return;

  glm::vec3 up = tile.normal;
  glm::vec3 temp = (std::abs(up.y) > 0.9f) ? glm::vec3(1.0f, 0.0f, 0.0f) : glm::vec3(0.0f, 1.0f, 0.0f);
  glm::vec3 tan = glm::normalize(glm::cross(up, temp));
  glm::vec3 bitan = glm::normalize(glm::cross(up, tan));

  glm::vec3 pos = tile.center * 1.01f;
  glm::vec4 postColor(0.85f, 0.85f, 0.88f, 1.0f);
  glm::vec4 panelColor(0.12f, 0.38f, 0.95f, 1.0f); // Solar Cell Blue

  // Central Pylon
  glm::vec3 top = pos + up * 0.045f;
  uint16_t pylonBase = static_cast<uint16_t>(verts.size());
  verts.push_back({top, up, postColor, {0.5f, 0.5f}});
  verts.push_back({pos, -up, postColor * 0.7f, {0.5f, 0.5f}});
  verts.push_back({top + tan * 0.008f, up, postColor, {0.5f, 0.5f}});
  verts.push_back({top - tan * 0.008f, up, postColor, {0.5f, 0.5f}});

  indices.push_back(pylonBase); indices.push_back(pylonBase + 2); indices.push_back(pylonBase + 1);
  indices.push_back(pylonBase); indices.push_back(pylonBase + 3); indices.push_back(pylonBase + 1);

  // Twin Solar Panels (Winged)
  float wingW = 0.038f;
  float wingH = 0.022f;

  for (int side = -1; side <= 1; side += 2) {
    if (verts.size() + 4 >= MAX_VERTICES || indices.size() + 6 >= MAX_INDICES) return;
    glm::vec3 wCenter = top + tan * (static_cast<float>(side) * 0.030f);
    uint16_t wBase = static_cast<uint16_t>(verts.size());

    glm::vec3 c0 = wCenter - tan * wingW * 0.5f - bitan * wingH * 0.5f + up * 0.005f;
    glm::vec3 c1 = wCenter + tan * wingW * 0.5f - bitan * wingH * 0.5f + up * 0.005f;
    glm::vec3 c2 = wCenter + tan * wingW * 0.5f + bitan * wingH * 0.5f - up * 0.005f;
    glm::vec3 c3 = wCenter - tan * wingW * 0.5f + bitan * wingH * 0.5f - up * 0.005f;

    verts.push_back({c0, up, panelColor, {0.5f, 0.5f}});
    verts.push_back({c1, up, panelColor * 1.2f, {0.5f, 0.5f}});
    verts.push_back({c2, up, panelColor * 0.9f, {0.5f, 0.5f}});
    verts.push_back({c3, up, panelColor, {0.5f, 0.5f}});

    indices.push_back(wBase); indices.push_back(wBase + 1); indices.push_back(wBase + 2);
    indices.push_back(wBase); indices.push_back(wBase + 2); indices.push_back(wBase + 3);
  }
}
