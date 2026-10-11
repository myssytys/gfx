#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <array>
#include <cstdint>
#include <string>
#include <vector>

struct Particle {
  alignas(16) glm::vec4 pos;   // xyz = position, w = current life
  alignas(16) glm::vec4 vel;   // xyz = velocity, w = max life
  alignas(16) glm::vec4 color; // rgba
  alignas(16) glm::vec4 scale; // x = size, y = rotation angle, z = rotSpeed, w = unused
};

struct ParticleComputePushConstants {
  alignas(16) glm::vec4 attractorPos; // xyz = attractor position, w = attractor strength
  float deltaTime;
  float time;
  uint32_t particleCount;
  float padding;
};

class ParticleSystem {
public:
  ParticleSystem(uint32_t particleCount = 65536);
  ~ParticleSystem();

  void init(VkDevice device,
            VkPhysicalDevice physicalDevice,
            VkCommandPool commandPool,
            VkQueue queue,
            VkRenderPass renderPass,
            const std::vector<VkBuffer> &uboBuffers,
            VkDeviceSize uboSize,
            uint32_t maxFramesInFlight);

  void recordCompute(VkCommandBuffer commandBuffer,
                     uint32_t currentFrame,
                     float deltaTime,
                     float totalTime,
                     const glm::vec3 &attractorPos = glm::vec3(0.0f, 0.5f, 0.0f),
                     float attractorStrength = 2.5f);

  void recordRender(VkCommandBuffer commandBuffer,
                    uint32_t currentFrame);

  void cleanup();

  uint32_t getParticleCount() const { return particleCount; }
  void setParticleCount(uint32_t count) { particleCount = count; }

private:
  uint32_t particleCount;
  uint32_t maxFramesInFlight = 2;

  VkDevice device = VK_NULL_HANDLE;
  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
  VkCommandPool commandPool = VK_NULL_HANDLE;
  VkQueue queue = VK_NULL_HANDLE;

  // Storage Buffers (SSBOs) for ping-pong simulation per frame in flight
  std::vector<VkBuffer> particleBuffers;
  std::vector<VkDeviceMemory> particleBuffersMemory;

  // Compute Pipeline
  VkDescriptorSetLayout computeDescriptorSetLayout = VK_NULL_HANDLE;
  VkPipelineLayout computePipelineLayout = VK_NULL_HANDLE;
  VkPipeline computePipeline = VK_NULL_HANDLE;
  VkDescriptorPool computeDescriptorPool = VK_NULL_HANDLE;
  std::vector<VkDescriptorSet> computeDescriptorSets;

  // Graphics / Billboard Pipeline
  VkDescriptorSetLayout graphicsDescriptorSetLayout = VK_NULL_HANDLE;
  VkPipelineLayout graphicsPipelineLayout = VK_NULL_HANDLE;
  VkPipeline graphicsPipeline = VK_NULL_HANDLE;
  VkDescriptorPool graphicsDescriptorPool = VK_NULL_HANDLE;
  std::vector<VkDescriptorSet> graphicsDescriptorSets;

  // Internal initialization helpers
  void createParticleBuffers();
  void createComputePipeline();
  void createComputeDescriptorSets();
  void createGraphicsPipeline(VkRenderPass renderPass);
  void createGraphicsDescriptorSets(const std::vector<VkBuffer> &uboBuffers,
                                    VkDeviceSize uboSize);

  uint32_t findMemoryType(uint32_t typeFilter,
                          VkMemoryPropertyFlags properties);
  void createBuffer(VkDeviceSize size,
                    VkBufferUsageFlags usage,
                    VkMemoryPropertyFlags properties,
                    VkBuffer &buffer,
                    VkDeviceMemory &bufferMemory);
  void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
  VkCommandBuffer beginSingleTimeCommands();
  void endSingleTimeCommands(VkCommandBuffer commandBuffer);

  VkShaderModule createShaderModule(const std::vector<char> &code);
  static std::vector<char> readFile(const std::string &filename);
};

#endif // PARTICLE_SYSTEM_H
