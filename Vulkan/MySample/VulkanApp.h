#include <vulkan/vulkan.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <iostream>
#include <vector>

#include <stdexcept>
#include <cstdlib>

#define NDEBUG 0

class VulkanApp {
public:
  VulkanApp();
  ~VulkanApp();
  void init();
  void run();
  void deInit();

private:
  bool checkValidationLayerSupport();
  void initGlfw();
  void initVulkan();
  void update();
  void cleanup();
  void deInitVulkan();
  void deInitGlfw();
  std::vector<const char*> getRequiredExtensions();
  void initDebugMessenger();
  void deInitDebugMessenger();
  VkResult createDebugUtilsMessengerEXT(VkInstance instance, 
                                      const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
                                      const VkAllocationCallbacks* pAllocator, 
                                      VkDebugUtilsMessengerEXT* pDebugMessenger);

private:
#if NDEBUG
  const bool enableValidationLayers_ = false;
#else
  const bool enableValidationLayers_ = true;
  const std::vector<const char*> validationLayers_;
#endif

  GLFWwindow* window_;
  VkInstance vkInstance_;
  VkDebugUtilsMessengerEXT debugMessenger_;
};