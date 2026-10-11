#include "ParticleSystem.h"

#include <array>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <random>
#include <stdexcept>

ParticleSystem::ParticleSystem(uint32_t particleCount)
    : particleCount(particleCount) {}

ParticleSystem::~ParticleSystem() {}

void ParticleSystem::init(VkDevice device,
                          VkPhysicalDevice physicalDevice,
                          VkCommandPool commandPool,
                          VkQueue queue,
                          VkRenderPass renderPass,
                          const std::vector<VkBuffer> &uboBuffers,
                          VkDeviceSize uboSize,
                          uint32_t maxFramesInFlight) {
  this->device = device;
  this->physicalDevice = physicalDevice;
  this->commandPool = commandPool;
  this->queue = queue;
  this->maxFramesInFlight = maxFramesInFlight;

  createParticleBuffers();
  createComputePipeline();
  createComputeDescriptorSets();
  createGraphicsPipeline(renderPass);
  createGraphicsDescriptorSets(uboBuffers, uboSize);
}

void ParticleSystem::createParticleBuffers() {
  VkDeviceSize bufferSize = sizeof(Particle) * particleCount;

  // Initialize random particle distribution on CPU for staging
  std::vector<Particle> initialParticles(particleCount);
  std::default_random_engine rndEngine(1337);
  std::uniform_real_distribution<float> rndDist(-1.0f, 1.0f);
  std::uniform_real_distribution<float> rndPos(-0.3f, 0.3f);
  std::uniform_real_distribution<float> rndLife(0.0f, 3.0f);

  for (uint32_t i = 0; i < particleCount; ++i) {
    float life = rndLife(rndEngine);
    float maxLife = 2.0f + std::abs(rndDist(rndEngine)) * 2.0f;

    initialParticles[i].pos = glm::vec4(
        rndPos(rndEngine), rndPos(rndEngine) * 0.5f, rndPos(rndEngine), life);

    float speed = 1.0f + std::abs(rndDist(rndEngine)) * 2.0f;
    glm::vec3 velDir = glm::normalize(
        glm::vec3(rndDist(rndEngine), std::abs(rndDist(rndEngine)) + 0.5f,
                  rndDist(rndEngine)));
    initialParticles[i].vel = glm::vec4(velDir * speed, maxLife);

    initialParticles[i].color =
        glm::vec4(0.1f, 0.5f + rndDist(rndEngine) * 0.2f, 1.0f, 1.0f);
    initialParticles[i].scale =
        glm::vec4(0.04f + std::abs(rndDist(rndEngine)) * 0.03f,
                  rndDist(rndEngine) * 3.14159f, rndDist(rndEngine) * 3.0f,
                  0.0f);
  }

  // Create staging buffer
  VkBuffer stagingBuffer;
  VkDeviceMemory stagingBufferMemory;
  createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                   VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
               stagingBuffer, stagingBufferMemory);

  void *data;
  vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
  memcpy(data, initialParticles.data(), (size_t)bufferSize);
  vkUnmapMemory(device, stagingBufferMemory);

  // Allocate storage buffers for each frame in flight
  particleBuffers.resize(maxFramesInFlight);
  particleBuffersMemory.resize(maxFramesInFlight);

  for (size_t i = 0; i < maxFramesInFlight; ++i) {
    createBuffer(bufferSize,
                 VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
                     VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                     VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, particleBuffers[i],
                 particleBuffersMemory[i]);

    copyBuffer(stagingBuffer, particleBuffers[i], bufferSize);
  }

  vkDestroyBuffer(device, stagingBuffer, nullptr);
  vkFreeMemory(device, stagingBufferMemory, nullptr);
}

void ParticleSystem::createComputePipeline() {
  // Descriptor set layout: Binding 0 = In SSBO, Binding 1 = Out SSBO
  std::array<VkDescriptorSetLayoutBinding, 2> layoutBindings{};

  layoutBindings[0].binding = 0;
  layoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
  layoutBindings[0].descriptorCount = 1;
  layoutBindings[0].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
  layoutBindings[0].pImmutableSamplers = nullptr;

  layoutBindings[1].binding = 1;
  layoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
  layoutBindings[1].descriptorCount = 1;
  layoutBindings[1].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
  layoutBindings[1].pImmutableSamplers = nullptr;

  VkDescriptorSetLayoutCreateInfo layoutInfo{};
  layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layoutInfo.bindingCount = static_cast<uint32_t>(layoutBindings.size());
  layoutInfo.pBindings = layoutBindings.data();

  if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr,
                                  &computeDescriptorSetLayout) != VK_SUCCESS) {
    throw std::runtime_error(
        "failed to create particle compute descriptor set layout!");
  }

  // Push constants for dynamic simulation parameters (time, dt, attractor)
  VkPushConstantRange pushConstantRange{};
  pushConstantRange.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
  pushConstantRange.offset = 0;
  pushConstantRange.size = sizeof(ParticleComputePushConstants);

  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = 1;
  pipelineLayoutInfo.pSetLayouts = &computeDescriptorSetLayout;
  pipelineLayoutInfo.pushConstantRangeCount = 1;
  pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

  if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr,
                                 &computePipelineLayout) != VK_SUCCESS) {
    throw std::runtime_error(
        "failed to create particle compute pipeline layout!");
  }

  auto computeShaderCode = readFile("shaders/particle_comp.spv");
  VkShaderModule computeShaderModule = createShaderModule(computeShaderCode);

  VkPipelineShaderStageCreateInfo computeShaderStageInfo{};
  computeShaderStageInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  computeShaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
  computeShaderStageInfo.module = computeShaderModule;
  computeShaderStageInfo.pName = "main";

  VkComputePipelineCreateInfo pipelineInfo{};
  pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
  pipelineInfo.layout = computePipelineLayout;
  pipelineInfo.stage = computeShaderStageInfo;

  if (vkCreateComputePipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo,
                               nullptr, &computePipeline) != VK_SUCCESS) {
    throw std::runtime_error("failed to create particle compute pipeline!");
  }

  vkDestroyShaderModule(device, computeShaderModule, nullptr);
}

void ParticleSystem::createComputeDescriptorSets() {
  std::array<VkDescriptorPoolSize, 1> poolSizes{};
  poolSizes[0].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
  poolSizes[0].descriptorCount =
      static_cast<uint32_t>(maxFramesInFlight * 2);

  VkDescriptorPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
  poolInfo.pPoolSizes = poolSizes.data();
  poolInfo.maxSets = static_cast<uint32_t>(maxFramesInFlight);

  if (vkCreateDescriptorPool(device, &poolInfo, nullptr,
                             &computeDescriptorPool) != VK_SUCCESS) {
    throw std::runtime_error(
        "failed to create particle compute descriptor pool!");
  }

  std::vector<VkDescriptorSetLayout> layouts(maxFramesInFlight,
                                             computeDescriptorSetLayout);
  VkDescriptorSetAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = computeDescriptorPool;
  allocInfo.descriptorSetCount = static_cast<uint32_t>(maxFramesInFlight);
  allocInfo.pSetLayouts = layouts.data();

  computeDescriptorSets.resize(maxFramesInFlight);
  if (vkAllocateDescriptorSets(device, &allocInfo,
                               computeDescriptorSets.data()) != VK_SUCCESS) {
    throw std::runtime_error(
        "failed to allocate particle compute descriptor sets!");
  }

  for (size_t i = 0; i < maxFramesInFlight; ++i) {
    size_t prevIndex = (i + maxFramesInFlight - 1) % maxFramesInFlight;

    VkDescriptorBufferInfo inBufferInfo{};
    inBufferInfo.buffer = particleBuffers[prevIndex];
    inBufferInfo.offset = 0;
    inBufferInfo.range = sizeof(Particle) * particleCount;

    VkDescriptorBufferInfo outBufferInfo{};
    outBufferInfo.buffer = particleBuffers[i];
    outBufferInfo.offset = 0;
    outBufferInfo.range = sizeof(Particle) * particleCount;

    std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

    descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[0].dstSet = computeDescriptorSets[i];
    descriptorWrites[0].dstBinding = 0;
    descriptorWrites[0].dstArrayElement = 0;
    descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    descriptorWrites[0].descriptorCount = 1;
    descriptorWrites[0].pBufferInfo = &inBufferInfo;

    descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[1].dstSet = computeDescriptorSets[i];
    descriptorWrites[1].dstBinding = 1;
    descriptorWrites[1].dstArrayElement = 0;
    descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    descriptorWrites[1].descriptorCount = 1;
    descriptorWrites[1].pBufferInfo = &outBufferInfo;

    vkUpdateDescriptorSets(device,
                           static_cast<uint32_t>(descriptorWrites.size()),
                           descriptorWrites.data(), 0, nullptr);
  }
}

void ParticleSystem::createGraphicsPipeline(VkRenderPass renderPass) {
  // Binding 0 = Camera UBO, Binding 1 = Particle SSBO
  std::array<VkDescriptorSetLayoutBinding, 2> layoutBindings{};

  layoutBindings[0].binding = 0;
  layoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  layoutBindings[0].descriptorCount = 1;
  layoutBindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
  layoutBindings[0].pImmutableSamplers = nullptr;

  layoutBindings[1].binding = 1;
  layoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
  layoutBindings[1].descriptorCount = 1;
  layoutBindings[1].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
  layoutBindings[1].pImmutableSamplers = nullptr;

  VkDescriptorSetLayoutCreateInfo layoutInfo{};
  layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layoutInfo.bindingCount = static_cast<uint32_t>(layoutBindings.size());
  layoutInfo.pBindings = layoutBindings.data();

  if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr,
                                  &graphicsDescriptorSetLayout) != VK_SUCCESS) {
    throw std::runtime_error(
        "failed to create particle graphics descriptor set layout!");
  }

  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = 1;
  pipelineLayoutInfo.pSetLayouts = &graphicsDescriptorSetLayout;
  pipelineLayoutInfo.pushConstantRangeCount = 0;

  if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr,
                             &graphicsPipelineLayout) != VK_SUCCESS) {
    throw std::runtime_error(
        "failed to create particle graphics pipeline layout!");
  }

  auto vertShaderCode = readFile("shaders/particle_vert.spv");
  auto fragShaderCode = readFile("shaders/particle_frag.spv");

  VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
  VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

  VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
  vertShaderStageInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vertShaderStageInfo.module = vertShaderModule;
  vertShaderStageInfo.pName = "main";

  VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
  fragShaderStageInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  fragShaderStageInfo.module = fragShaderModule;
  fragShaderStageInfo.pName = "main";

  VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo,
                                                    fragShaderStageInfo};

  // Procedural vertex generation (quad corners from gl_VertexIndex)
  VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
  vertexInputInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInputInfo.vertexBindingDescriptionCount = 0;
  vertexInputInfo.pVertexBindingDescriptions = nullptr;
  vertexInputInfo.vertexAttributeDescriptionCount = 0;
  vertexInputInfo.pVertexAttributeDescriptions = nullptr;

  VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
  inputAssembly.sType =
      VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
  inputAssembly.primitiveRestartEnable = VK_FALSE;

  VkPipelineViewportStateCreateInfo viewportState{};
  viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportState.viewportCount = 1;
  viewportState.scissorCount = 1;

  VkPipelineRasterizationStateCreateInfo rasterizer{};
  rasterizer.sType =
      VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer.depthClampEnable = VK_FALSE;
  rasterizer.rasterizerDiscardEnable = VK_FALSE;
  rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizer.lineWidth = 1.0f;
  rasterizer.cullMode = VK_CULL_MODE_NONE;
  rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
  rasterizer.depthBiasEnable = VK_FALSE;

  VkPipelineMultisampleStateCreateInfo multisampling{};
  multisampling.sType =
      VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling.sampleShadingEnable = VK_FALSE;
  multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

  // Additive Alpha Blending for glowing particles
  VkPipelineColorBlendAttachmentState colorBlendAttachment{};
  colorBlendAttachment.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  colorBlendAttachment.blendEnable = VK_TRUE;
  colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
  colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE; // Additive
  colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
  colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
  colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
  colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

  VkPipelineColorBlendStateCreateInfo colorBlending{};
  colorBlending.sType =
      VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlending.logicOpEnable = VK_FALSE;
  colorBlending.attachmentCount = 1;
  colorBlending.pAttachments = &colorBlendAttachment;

  // Depth test enabled, but depth write disabled so particles do not occlude geometry behind them
  VkPipelineDepthStencilStateCreateInfo depthStencil{};
  depthStencil.sType =
      VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  depthStencil.depthTestEnable = VK_TRUE;
  depthStencil.depthWriteEnable = VK_FALSE;
  depthStencil.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
  depthStencil.depthBoundsTestEnable = VK_FALSE;
  depthStencil.stencilTestEnable = VK_FALSE;

  std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT,
                                               VK_DYNAMIC_STATE_SCISSOR};
  VkPipelineDynamicStateCreateInfo dynamicState{};
  dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamicState.dynamicStateCount =
      static_cast<uint32_t>(dynamicStates.size());
  dynamicState.pDynamicStates = dynamicStates.data();

  VkGraphicsPipelineCreateInfo pipelineInfo{};
  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.stageCount = 2;
  pipelineInfo.pStages = shaderStages;
  pipelineInfo.pVertexInputState = &vertexInputInfo;
  pipelineInfo.pInputAssemblyState = &inputAssembly;
  pipelineInfo.pViewportState = &viewportState;
  pipelineInfo.pRasterizationState = &rasterizer;
  pipelineInfo.pMultisampleState = &multisampling;
  pipelineInfo.pDepthStencilState = &depthStencil;
  pipelineInfo.pColorBlendState = &colorBlending;
  pipelineInfo.pDynamicState = &dynamicState;
  pipelineInfo.layout = graphicsPipelineLayout;
  pipelineInfo.renderPass = renderPass;
  pipelineInfo.subpass = 0;

  if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo,
                                nullptr, &graphicsPipeline) != VK_SUCCESS) {
    throw std::runtime_error(
        "failed to create particle graphics pipeline!");
  }

  vkDestroyShaderModule(device, fragShaderModule, nullptr);
  vkDestroyShaderModule(device, vertShaderModule, nullptr);
}

void ParticleSystem::createGraphicsDescriptorSets(
    const std::vector<VkBuffer> &uboBuffers, VkDeviceSize uboSize) {
  std::array<VkDescriptorPoolSize, 2> poolSizes{};
  poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  poolSizes[0].descriptorCount = static_cast<uint32_t>(maxFramesInFlight);
  poolSizes[1].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
  poolSizes[1].descriptorCount = static_cast<uint32_t>(maxFramesInFlight);

  VkDescriptorPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
  poolInfo.pPoolSizes = poolSizes.data();
  poolInfo.maxSets = static_cast<uint32_t>(maxFramesInFlight);

  if (vkCreateDescriptorPool(device, &poolInfo, nullptr,
                             &graphicsDescriptorPool) != VK_SUCCESS) {
    throw std::runtime_error(
        "failed to create particle graphics descriptor pool!");
  }

  std::vector<VkDescriptorSetLayout> layouts(maxFramesInFlight,
                                             graphicsDescriptorSetLayout);
  VkDescriptorSetAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = graphicsDescriptorPool;
  allocInfo.descriptorSetCount = static_cast<uint32_t>(maxFramesInFlight);
  allocInfo.pSetLayouts = layouts.data();

  graphicsDescriptorSets.resize(maxFramesInFlight);
  if (vkAllocateDescriptorSets(device, &allocInfo,
                               graphicsDescriptorSets.data()) != VK_SUCCESS) {
    throw std::runtime_error(
        "failed to allocate particle graphics descriptor sets!");
  }

  for (size_t i = 0; i < maxFramesInFlight; ++i) {
    VkDescriptorBufferInfo uboBufferInfo{};
    uboBufferInfo.buffer = uboBuffers[i];
    uboBufferInfo.offset = 0;
    uboBufferInfo.range = uboSize;

    VkDescriptorBufferInfo particleBufferInfo{};
    particleBufferInfo.buffer = particleBuffers[i];
    particleBufferInfo.offset = 0;
    particleBufferInfo.range = sizeof(Particle) * particleCount;

    std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

    descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[0].dstSet = graphicsDescriptorSets[i];
    descriptorWrites[0].dstBinding = 0;
    descriptorWrites[0].dstArrayElement = 0;
    descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrites[0].descriptorCount = 1;
    descriptorWrites[0].pBufferInfo = &uboBufferInfo;

    descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[1].dstSet = graphicsDescriptorSets[i];
    descriptorWrites[1].dstBinding = 1;
    descriptorWrites[1].dstArrayElement = 0;
    descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    descriptorWrites[1].descriptorCount = 1;
    descriptorWrites[1].pBufferInfo = &particleBufferInfo;

    vkUpdateDescriptorSets(device,
                           static_cast<uint32_t>(descriptorWrites.size()),
                           descriptorWrites.data(), 0, nullptr);
  }
}

void ParticleSystem::recordCompute(VkCommandBuffer commandBuffer,
                                   uint32_t currentFrame,
                                   float deltaTime,
                                   float totalTime,
                                   const glm::vec3 &attractorPos,
                                   float attractorStrength) {
  vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE,
                    computePipeline);

  vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE,
                          computePipelineLayout, 0, 1,
                          &computeDescriptorSets[currentFrame], 0, nullptr);

  ParticleComputePushConstants pushConstants{};
  pushConstants.attractorPos =
      glm::vec4(attractorPos, attractorStrength);
  pushConstants.deltaTime = deltaTime;
  pushConstants.time = totalTime;
  pushConstants.particleCount = particleCount;
  pushConstants.padding = 0.0f;

  vkCmdPushConstants(commandBuffer, computePipelineLayout,
                     VK_SHADER_STAGE_COMPUTE_BIT, 0,
                     sizeof(ParticleComputePushConstants), &pushConstants);

  uint32_t groupCountX = (particleCount + 255) / 256;
  vkCmdDispatch(commandBuffer, groupCountX, 1, 1);

  // Synchronization barrier: Ensure compute shader finishes writing before vertex shader reads
  VkBufferMemoryBarrier barrier{};
  barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
  barrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
  barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
  barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.buffer = particleBuffers[currentFrame];
  barrier.offset = 0;
  barrier.size = VK_WHOLE_SIZE;

  vkCmdPipelineBarrier(
      commandBuffer,
      VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, // Producer stage
      VK_PIPELINE_STAGE_VERTEX_SHADER_BIT,  // Consumer stage
      0,
      0, nullptr,
      1, &barrier,
      0, nullptr);
}

void ParticleSystem::recordRender(VkCommandBuffer commandBuffer,
                                  uint32_t currentFrame) {
  vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                    graphicsPipeline);

  vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                          graphicsPipelineLayout, 0, 1,
                          &graphicsDescriptorSets[currentFrame], 0, nullptr);

  // Draw 4 vertices per billboard quad (triangle strip), instanced for all particles
  vkCmdDraw(commandBuffer, 4, particleCount, 0, 0);
}

void ParticleSystem::cleanup() {
  if (device == VK_NULL_HANDLE) return;

  vkDestroyPipeline(device, graphicsPipeline, nullptr);
  vkDestroyPipelineLayout(device, graphicsPipelineLayout, nullptr);
  vkDestroyDescriptorPool(device, graphicsDescriptorPool, nullptr);
  vkDestroyDescriptorSetLayout(device, graphicsDescriptorSetLayout, nullptr);

  vkDestroyPipeline(device, computePipeline, nullptr);
  vkDestroyPipelineLayout(device, computePipelineLayout, nullptr);
  vkDestroyDescriptorPool(device, computeDescriptorPool, nullptr);
  vkDestroyDescriptorSetLayout(device, computeDescriptorSetLayout, nullptr);

  for (size_t i = 0; i < particleBuffers.size(); ++i) {
    vkDestroyBuffer(device, particleBuffers[i], nullptr);
    vkFreeMemory(device, particleBuffersMemory[i], nullptr);
  }
  particleBuffers.clear();
  particleBuffersMemory.clear();

  device = VK_NULL_HANDLE;
}

uint32_t ParticleSystem::findMemoryType(uint32_t typeFilter,
                                       VkMemoryPropertyFlags properties) {
  VkPhysicalDeviceMemoryProperties memProperties;
  vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

  for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
    if ((typeFilter & (1 << i)) &&
        (memProperties.memoryTypes[i].propertyFlags & properties) ==
            properties) {
      return i;
    }
  }

  throw std::runtime_error("failed to find suitable memory type!");
}

void ParticleSystem::createBuffer(VkDeviceSize size,
                                  VkBufferUsageFlags usage,
                                  VkMemoryPropertyFlags properties,
                                  VkBuffer &buffer,
                                  VkDeviceMemory &bufferMemory) {
  VkBufferCreateInfo bufferInfo{};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = size;
  bufferInfo.usage = usage;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
    throw std::runtime_error("failed to create buffer!");
  }

  VkMemoryRequirements memRequirements;
  vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

  VkMemoryAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex =
      findMemoryType(memRequirements.memoryTypeBits, properties);

  if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to allocate buffer memory!");
  }

  vkBindBufferMemory(device, buffer, bufferMemory, 0);
}

VkCommandBuffer ParticleSystem::beginSingleTimeCommands() {
  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandPool = commandPool;
  allocInfo.commandBufferCount = 1;

  VkCommandBuffer cmdBuffer;
  vkAllocateCommandBuffers(device, &allocInfo, &cmdBuffer);

  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  vkBeginCommandBuffer(cmdBuffer, &beginInfo);
  return cmdBuffer;
}

void ParticleSystem::endSingleTimeCommands(VkCommandBuffer cmdBuffer) {
  vkEndCommandBuffer(cmdBuffer);

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &cmdBuffer;

  vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
  vkQueueWaitIdle(queue);

  vkFreeCommandBuffers(device, commandPool, 1, &cmdBuffer);
}

void ParticleSystem::copyBuffer(VkBuffer srcBuffer,
                                VkBuffer dstBuffer,
                                VkDeviceSize size) {
  VkCommandBuffer cmdBuffer = beginSingleTimeCommands();

  VkBufferCopy copyRegion{};
  copyRegion.size = size;
  vkCmdCopyBuffer(cmdBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

  endSingleTimeCommands(cmdBuffer);
}

VkShaderModule ParticleSystem::createShaderModule(
    const std::vector<char> &code) {
  VkShaderModuleCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = code.size();
  createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

  VkShaderModule shaderModule;
  if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create shader module!");
  }

  return shaderModule;
}

std::vector<char> ParticleSystem::readFile(const std::string &filename) {
  std::ifstream file(filename, std::ios::ate | std::ios::binary);

  if (!file.is_open()) {
    throw std::runtime_error("failed to open file: " + filename);
  }

  size_t fileSize = (size_t)file.tellg();
  std::vector<char> buffer(fileSize);

  file.seekg(0);
  file.read(buffer.data(), fileSize);
  file.close();

  return buffer;
}
