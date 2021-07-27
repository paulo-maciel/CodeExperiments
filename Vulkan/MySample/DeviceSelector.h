#include <vulkan/vulkan.h>

#include <QueueSelector.h>

#include <iostream>
#include <vector>

#include <stdexcept>
#include <cstdlib>

#define NDEBUG 0

class DeviceSelector {
public:
  DeviceSelector();
  ~DeviceSelector();

  void select(const VkInstance& vkInstance);
  int rateDevice(const VkPhysicalDevice& physicalDevice) const;

private:
 
  VkPhysicalDevice physicalDevice_;
  QueueSelector queueSelector_;

};