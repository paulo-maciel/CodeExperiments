#pragma once

#include <Device.h>

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

class Buffer {
public:
  Buffer(std::shared_ptr<Device> device);
  ~Buffer();

  // Create a single vertex buffer
  void create(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory);
  void destroy();

protected:
  uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

  std::shared_ptr<Device> device_;
};