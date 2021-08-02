#include <vulkan/vulkan.h>


#include <vector>

#define NDEBUG 0

class SwapChain {
public:
  SwapChain(VkSurfaceKHR surface);
  ~SwapChain();

  struct Details {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
  };

  Details getDetails(VkPhysicalDevice device);
  

private:
  VkSurfaceKHR vkSurface_;
};