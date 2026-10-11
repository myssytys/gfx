#ifndef UNIT_MANAGER_H
#define UNIT_MANAGER_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <unordered_map>
#include "Sphere.h"

enum class UnitType {
  SCOUT,
  WARRIOR,
  SETTLER
};

struct Unit {
  int id;
  std::string name;
  UnitType type;
  int owner; // 0 = Player, 1 = Rival Faction
  int tileId;
  int movementPoints;
  int maxMovementPoints;
  int health;
  int maxHealth;
  glm::vec4 color;

  // Animation & position state
  bool isMoving = false;
  std::vector<int> path;       // Sequence of tile IDs
  size_t currentPathIdx = 0;
  float moveProgress = 0.0f;   // 0.0 -> 1.0 along arc
  glm::vec3 currentPosition;   // Local planet space
  glm::vec3 currentNormal;
};

class UnitManager {
public:
  std::vector<Unit> units;
  int selectedUnitId = -1;
  int currentTurn = 1;

  // Vulkan buffers for unit meshes
  VkDevice device = VK_NULL_HANDLE;
  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

  VkBuffer vertexBuffer = VK_NULL_HANDLE;
  VkDeviceMemory vertexBufferMemory = VK_NULL_HANDLE;
  void *vertexBufferMapped = nullptr;

  VkBuffer indexBuffer = VK_NULL_HANDLE;
  VkDeviceMemory indexBufferMemory = VK_NULL_HANDLE;
  void *indexBufferMapped = nullptr;

  uint32_t indexCount = 0;

  static const size_t MAX_UNITS = 64;
  static const size_t MAX_VERTICES = 4096;
  static const size_t MAX_INDICES = 8192;

  UnitManager();
  ~UnitManager();

  void init(VkDevice dev, VkPhysicalDevice physDev);
  void cleanup();

  // Spawning
  int spawnUnit(UnitType type, int owner, int tileId, const Sphere &planet);
  void spawnDefaultUnits(const Sphere &planet);

  // Queries
  Unit *getUnit(int unitId);
  const Unit *getUnit(int unitId) const;
  Unit *getUnitAtTile(int tileId);
  const Unit *getUnitAtTile(int tileId) const;
  Unit *getSelectedUnit();
  void selectUnit(int unitId);
  void deselectUnit();
  int selectNextPlayerUnit();

  // Turn management
  void nextTurn();

  // Movement & Pathfinding
  bool isTilePassable(int tileId, const Sphere &planet, const Unit &unit) const;
  int getTileMovementCost(int tileId, const Sphere &planet, const Unit &unit) const;
  std::vector<int> getReachableTiles(int unitId, const Sphere &planet) const;
  std::vector<int> findPath(int unitId, int targetTileId, const Sphere &planet) const;
  bool moveUnit(int unitId, int targetTileId, const Sphere &planet);

  // Update & Render
  void update(float deltaTime, float totalTime, const Sphere &planet);
  void recordRender(VkCommandBuffer commandBuffer);

private:
  void createBuffers();
  uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
  void buildUnitMeshes(float totalTime, const Sphere &planet);

  void addScoutMesh(const Unit &unit, const glm::vec3 &pos, const glm::vec3 &normal,
                    float totalTime, bool isSelected,
                    std::vector<Sphere::Vertex> &verts, std::vector<uint16_t> &indices);

  void addWarriorMesh(const Unit &unit, const glm::vec3 &pos, const glm::vec3 &normal,
                      float totalTime, bool isSelected,
                      std::vector<Sphere::Vertex> &verts, std::vector<uint16_t> &indices);

  void addSettlerMesh(const Unit &unit, const glm::vec3 &pos, const glm::vec3 &normal,
                      float totalTime, bool isSelected,
                      std::vector<Sphere::Vertex> &verts, std::vector<uint16_t> &indices);

  void addSelectionRing(const glm::vec3 &pos, const glm::vec3 &normal,
                        float totalTime,
                        std::vector<Sphere::Vertex> &verts, std::vector<uint16_t> &indices);
};

#endif
