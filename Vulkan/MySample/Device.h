#include <vulkan/vulkan.h>

#include <QueueSelector.h>

#include <iostream>
#include <vector>

#include <stdexcept>
#include <cstdlib>

#define NDEBUG 0

class Device {
public:
  Device(VkInstance vkInstance);
  ~Device();

  void create(const std::vector<const char*>& validationLayers);

private:
  void selectPhysical();
  int rateDevice(const VkPhysicalDevice& physicalDevice) const;
  void createLogical(const std::vector<const char*>& validationLayers);

private:
  VkInstance vkInstance_;

  // Physical and connected logical device.
  VkPhysicalDevice physicalDevice_;
  VkDevice device_;

  QueueSelector queueSelector_;
};