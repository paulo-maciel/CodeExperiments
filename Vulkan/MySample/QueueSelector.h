#pragma once

#include <vulkan/vulkan.h>


#include <experimental/optional>
#include <vector>
#include <memory>

class Device;

#define NDEBUG 0

class QueueSelector {
public:
  
  QueueSelector(VkSurfaceKHR vkSurface, std::shared_ptr<Device> device);
  ~QueueSelector();

  struct QueueFamilyIndices {
    std::experimental::optional<uint32_t> graphicsFamily;
    std::experimental::optional<uint32_t> presentFamily;
    
    bool hasValue();
    uint32_t value();
  };

  VkDeviceQueueCreateInfo *getQueuesCreateInfo();
  uint32_t getQueuesCreateInfoSize() const;

  bool hasValidFamilies(VkPhysicalDevice physicalDevice);
  QueueFamilyIndices findFamilies(const VkPhysicalDevice& device) const;

  void cacheQueues(VkDevice device);
  VkQueue getGraphicsQueue() const;
  VkQueue getPresentQueue() const;

private:
  VkQueue graphicsQueue_;
  VkQueue presentQueue_;
  VkSurfaceKHR vkSurface_;
  std::vector<VkDeviceQueueCreateInfo> queuesCreateInfo_;
  float queuePriority_;

  std::shared_ptr<Device> device_;

};