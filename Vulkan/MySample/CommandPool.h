#pragma once

#include <vulkan/vulkan.h>

#include <SwapChain.h>
#include <QueueSelector.h>
#include <GraphicsPipeline.h>

#include <vector>
#include <string>
#include <memory>

#define NDEBUG 0

class VertexBuffer;

class CommandPool {
public:
  CommandPool(std::shared_ptr<Device> device);
  ~CommandPool();

  void create();
  void destroy();

  void createCommandBuffers(std::shared_ptr<VertexBuffer> vertexBuffer);
  VkCommandBuffer *getCommandBuffers();

  VkCommandPool getCommandPool() const;

private:
  std::shared_ptr<Device> device_;

  VkCommandPool commandPool_;
  std::vector<VkCommandBuffer> commandBuffers_;
};