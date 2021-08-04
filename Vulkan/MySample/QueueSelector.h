#pragma once

#include <vulkan/vulkan.h>

#include <experimental/optional>
#include <vector>

#define NDEBUG 0

class QueueSelector {
public:
  friend class Device;
  
  QueueSelector(VkSurfaceKHR vkSurface);
  ~QueueSelector();

  struct QueueFamilyIndices {
    std::experimental::optional<uint32_t> graphicsFamily;
    std::experimental::optional<uint32_t> presentFamily;
    
    bool hasValue();
    uint32_t value();
  };

  VkDeviceQueueCreateInfo *getQueuesCreateInfo(VkPhysicalDevice physicalDevice, VkDevice device);
  uint32_t getQueuesCreateInfoSize() const;

  bool hasValidFamilies(VkPhysicalDevice physicalDevice);

private:
  QueueFamilyIndices findFamilies(const VkPhysicalDevice& device) const;

  VkQueue graphicsQueue_;
  VkQueue presentQueue_;
  VkSurfaceKHR vkSurface_;
  std::vector<VkDeviceQueueCreateInfo> queuesCreateInfo_;
  float queuePriority_;

};