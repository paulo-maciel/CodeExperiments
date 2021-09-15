#pragma once

#include <Image.h>

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

#include <memory>
#include <vector>

class Device;

class DepthStencil : public Image {
public:
  DepthStencil(std::shared_ptr<Device> device);
  ~DepthStencil();

  bool create(uint32_t width, uint32_t height);
  void destroy();
  bool hasStencilComponent(VkFormat format);

private:
  VkFormat findDepthFormat();
  VkFormat findSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
  void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

private : 
  std::shared_ptr<Device> device_;

  VkImage depthImage_;
  VkDeviceMemory depthImageMemory_;
  VkImageView depthImageView_;
};