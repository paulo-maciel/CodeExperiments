#include <vulkan/vulkan.h>

#include <Device.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <iostream>
#include <vector>
#include <memory>

#include <stdexcept>
#include <cstdlib>
#include <memory>

#define NDEBUG 0

class VulkanApp {
public:
  VulkanApp();
  ~VulkanApp();
  void init();
  void run();
  void deInit();
  void drawFrame();

private:
  bool checkValidationLayerSupport();
  void initGlfw();
  void createSurface();
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
  const std::vector<const char*> validationLayers_;

  GLFWwindow* window_;
  VkInstance vkInstance_;
  VkDebugUtilsMessengerEXT debugMessenger_;
  
  // VkSurfaceKHR object and its usage is platform agnostic, its
  // creation isn’t because it depends on window system details. 
  // For example, it needs the HWND and HMODULE handles on Windows.
  VkSurfaceKHR vkSurface_;
  std::shared_ptr<Device> device_;

  uint32_t currentFrame_;
  uint8_t maxFramesInFlight_;
};