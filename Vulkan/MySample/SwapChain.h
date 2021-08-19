#pragma once

#include <vulkan/vulkan.h>

#include <QueueSelector.h>


#include <vector>

#define NDEBUG 0

class Device;

class SwapChain {
public:
  SwapChain(VkSurfaceKHR surface);
  ~SwapChain();

  struct Details {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
  };

  bool create(Device *device, QueueSelector::QueueFamilyIndices familyIndexes);
  void destroy();

  Details getDetails(VkPhysicalDevice device) const;
  VkExtent2D getExtent2D() const;
  VkSurfaceFormatKHR getFormat() const;
  VkSwapchainKHR getSwapChain() const;

  std::vector<VkImageView> getImagesView() const;
  std::vector<VkFramebuffer> getFrameBuffers() const;

  void createFrameBuffers(VkRenderPass renderPass);

private:
  VkSurfaceFormatKHR selectSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats) const;
  VkPresentModeKHR   selectSwapPresentMode(const std::vector<VkPresentModeKHR>& presentModes) const;
  // Resolution of the swap chain images and typically, the same as the resolution of the window.
  VkExtent2D selectSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) const;
  
private:
  VkSurfaceKHR vkSurface_;
  VkSwapchainKHR vkSwapChain_;
  VkDevice device_;

  VkSurfaceFormatKHR format_;
  VkPresentModeKHR presentMode_;
  VkExtent2D extent2D_;  

  // Images created by the implementation currently in this swap chain.
  std::vector<VkImage> images_;
  std::vector<VkImageView> imagesView_;

  // These are the frame buffers associated to the views vector above.
  std::vector<VkFramebuffer> frameBuffers_;
};