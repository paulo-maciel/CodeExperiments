#pragma once

#include <Device.h>

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

class CommandPool;
class QueueSelector;

class Buffer {
public:
  Buffer(std::shared_ptr<Device> device, std::shared_ptr<CommandPool> commandPool, std::shared_ptr<QueueSelector> queueSelector);
  ~Buffer();

  static uint32_t findMemoryType(VkPhysicalDevice device, uint32_t typeFilter, VkMemoryPropertyFlags properties);

  // Create a single vertex buffer
  void create(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory);
  void destroy();

protected:
  // TODO: Rename below to getCommandBuffer and submitCommandBuffer, respectivelly.
  VkCommandBuffer beginSingleTimeCommands();
  void endSingleTimeCommands(VkCommandBuffer commandBuffer);

  std::shared_ptr<Device> device_;
  std::shared_ptr<CommandPool> commandPool_;
  std::shared_ptr<QueueSelector> queueSelector_;
};