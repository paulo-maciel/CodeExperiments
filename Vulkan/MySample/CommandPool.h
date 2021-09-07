#pragma once

#include <vulkan/vulkan.h>


#include <vector>
#include <string>
#include <memory>

#define NDEBUG 0

class Device;
class VertexBuffer;
class SwapChain;
class GraphicsPipeline;
class UniformBuffer;

class CommandPool {
public:
  CommandPool(std::shared_ptr<Device> device,
              std::shared_ptr<SwapChain> swapChain,
              std::shared_ptr<GraphicsPipeline> graphicsPipeline);
  ~CommandPool();

  void create();
  void destroy();

  void createCommandBuffers(std::shared_ptr<VertexBuffer> vertexBuffer);
  VkCommandBuffer *getCommandBuffers();

  void createDescriptorPool();
  void createDescriptorSets(std::shared_ptr<UniformBuffer> uniformBuffer);
  std::vector<VkDescriptorSet> getDescriptorSets() const;

  VkCommandPool getCommandPool() const;

private:
  std::shared_ptr<Device> device_;
  std::shared_ptr<SwapChain> swapChain_;
  std::shared_ptr<GraphicsPipeline> graphicsPipeline_;

  VkCommandPool commandPool_;
  std::vector<VkCommandBuffer> commandBuffers_;

  VkDescriptorPool descriptorPool_;
  std::vector<VkDescriptorSet> descriptorSets_;
};