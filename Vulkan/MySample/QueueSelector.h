#include <vulkan/vulkan.h>

#include <experimental/optional>

#define NDEBUG 0

class QueueSelector {
public:
  QueueSelector(VkSurfaceKHR vkSurface);
  ~QueueSelector();

  struct QueueFamilyIndices {
    std::experimental::optional<uint32_t> graphicsFamily;
    std::experimental::optional<uint32_t> presentFamily;
    
    bool hasValue();
    uint32_t value();
  };

  const VkDeviceQueueCreateInfo& getQueueCreateInfo(VkPhysicalDevice physicalDevice);
  bool hasValidFamilies(VkPhysicalDevice physicalDevice);

private:
  QueueFamilyIndices findFamilies(const VkPhysicalDevice& device) const;

  VkQueue vkQueue_;
  VkSurfaceKHR vkSurface_;
  VkDeviceQueueCreateInfo queueCreateInfo_;
  float queuePriority_;

};