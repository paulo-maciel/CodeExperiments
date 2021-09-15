#pragma once

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

#include <memory>

class Device;

class Image {
public:

  Image();
  ~Image();

  bool create(std::shared_ptr<Device> device, uint32_t width, uint32_t height, 
              VkFormat format, VkImageTiling tiling,
              VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
              VkImage &image, VkDeviceMemory &imageMemory);

};