#pragma once

#include <Buffer.h>

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

#include <vector>
#include <array>
#include <memory>

class Device;
class SwapChain;

class UniformBuffer : public Buffer {
public:
  // TODO: This is app data or not.
  struct UniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
  };

  UniformBuffer(std::shared_ptr<Device> device, std::shared_ptr<CommandPool> commandPool, std::shared_ptr<QueueSelector> queueSelector, std::shared_ptr<SwapChain> swapChain);
  ~UniformBuffer();

  std::vector<VkBuffer> getUniformBuffers() const;

  bool create();
  void destroy();

  bool update(uint32_t currentImage);

private:

  std::shared_ptr<SwapChain> swapChain_;

  std::vector<VkBuffer> uniformBuffers_;
  std::vector<VkDeviceMemory> uniformBuffersMemory_;

  VkCommandPool commandPool_;
  VkQueue graphicsQueue_;
};