#pragma once

#include <Image.h>

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

#include <memory>

class Device;

class DepthStencil : public Image {
public:
  DepthStencil();
  ~DepthStencil();

  bool create(std::shared_ptr<Device> device, uint32_t width, uint32_t height);
  void destroy();

private:
  VkFormat findDepthFormat();

private:
  VkImage depthImage_;
  VkDeviceMemory depthImageMemory_;
  VkImageView depthImageView_;
};