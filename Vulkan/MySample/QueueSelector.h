#include <vulkan/vulkan.h>

#include <experimental/optional>

#define NDEBUG 0

class QueueSelector {
public:
  QueueSelector();
  ~QueueSelector();

  struct QueueFamilyIndex {
    std::experimental::optional<uint32_t> graphicsFamily;
    bool hasValue();
    uint32_t value();
  };

  const VkDeviceQueueCreateInfo& getQueueCreateInfo(VkPhysicalDevice physicalDevice);
  bool hasValidFamily(VkPhysicalDevice physicalDevice);

private:
  QueueFamilyIndex findFamily(const VkPhysicalDevice& device) const;

  VkQueue vkQueue_;
  VkDeviceQueueCreateInfo queueCreateInfo_;
  float queuePriority_;

};