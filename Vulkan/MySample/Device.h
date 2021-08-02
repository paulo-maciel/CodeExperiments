#include <vulkan/vulkan.h>

#include <QueueSelector.h>
#include <SwapChain.h>

#include <iostream>
#include <vector>

#include <stdexcept>
#include <cstdlib>
#include <memory>

#define NDEBUG 0

class Device {
public:
  Device(VkInstance vkInstance, VkSurfaceKHR vkSurface);
  ~Device();

  void create(const std::vector<const char*>& validationLayers);

private:
  void selectPhysical();
  int rateDevice(const VkPhysicalDevice& physicalDevice) const;
  void createLogical(const std::vector<const char*>& validationLayers);
  bool checkForRequiredExtension(VkPhysicalDevice device) const;

private:
  VkInstance vkInstance_;

  // Physical device.
  VkPhysicalDevice physicalDevice_;
  // Logical device.
  VkDevice device_;

  VkSurfaceKHR vkSurface_;

  // TODO: Set the device features.
  VkPhysicalDeviceFeatures deviceFeatures_;

  std::unique_ptr<QueueSelector> queueSelector_;
  std::unique_ptr<SwapChain> swapChain_;

  const std::vector<const char*> requiredExtensions_;
};