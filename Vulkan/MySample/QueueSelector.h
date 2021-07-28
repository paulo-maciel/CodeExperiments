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

  QueueFamilyIndex findFamily(const VkPhysicalDevice& device);

private:


};