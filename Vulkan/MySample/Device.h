#pragma once

#include <vulkan/vulkan.h>

class QueueSelector;
class SwapChain;
class GraphicsPipeline;
class CommandPool;
class DescriptorPool;
class SyncObjects;
class VertexBuffer;
class UniformBuffer;
class TextureImage;
class TextureImageSampler;

#include <iostream>
#include <vector>

#include <stdexcept>
#include <cstdlib>
#include <memory>

#define NDEBUG 0

class Device : public std::enable_shared_from_this<Device> {
public:
  Device(VkInstance vkInstance, VkSurfaceKHR vkSurface);
  ~Device();

  std::shared_ptr<Device> getPtr() {
      return shared_from_this();
  }

  void create(const std::vector<const char*>& validationLayers);
  void destroy();

  VkPhysicalDevice getPhysicalDevice() const;
  VkDevice         getLogicalDevice() const;

  std::shared_ptr<QueueSelector> getQueueSelector() const;
  std::shared_ptr<SwapChain> getSwapChain() const;
  std::shared_ptr<GraphicsPipeline> getGraphicsPipeline() const;
  std::shared_ptr<CommandPool> getCommandPool() const;
  std::shared_ptr<SyncObjects> getSyncObjects() const;
  std::shared_ptr<UniformBuffer> getUniformBuffer() const;

private:
  void selectPhysical();
  int rateDevice(const VkPhysicalDevice& physicalDevice);
  void createLogical(const std::vector<const char*>& validationLayers);
  bool checkForRequiredExtension(VkPhysicalDevice device) const;

private:
  VkInstance vkInstance_;

  // Physical device.
  VkPhysicalDevice physicalDevice_;
  
  // Logical device.
  VkDevice device_;

  VkSurfaceKHR vkSurface_;

  VkPhysicalDeviceFeatures requestedDeviceFeatures_;
  VkPhysicalDeviceFeatures deviceFeatures_;

  std::shared_ptr<QueueSelector> queueSelector_;
  std::shared_ptr<SwapChain> swapChain_;
  std::shared_ptr<GraphicsPipeline> graphicsPipeline_;
  std::shared_ptr<CommandPool> commandPool_;
  std::shared_ptr<DescriptorPool> descriptorPool_;
  std::shared_ptr<SyncObjects> syncObjects_;
  std::shared_ptr<VertexBuffer> vertexBuffer_;
  std::shared_ptr<UniformBuffer> uniformBuffer_;
  std::shared_ptr<TextureImage> textureImage_;
  std::shared_ptr<TextureImageSampler> textureImageSampler_;

  const std::vector<const char*> requiredExtensions_;
};