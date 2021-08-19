#pragma once

#include <vulkan/vulkan.h>

#include <vector>
#include <memory>

#define NDEBUG 0

class Device;

class SyncObjects {
public:
  
  SyncObjects(std::shared_ptr<Device> device);
  ~SyncObjects();

  bool create();
  bool destroy();

  std::vector<VkSemaphore> &getImageAvailableSemaphores();
  std::vector<VkSemaphore> &getRenderFinishedSemaphores();
  std::vector<VkFence> &getInflightFences();
  std::vector<VkFence> &getImagesInflight();

private:
  std::shared_ptr<Device> device_;
  std::vector<VkSemaphore> imageAvailableSemaphores_;
  std::vector<VkSemaphore> renderFinishedSemaphores_;

  std::vector<VkFence> inFlightFences_;
  std::vector<VkFence> imagesInFlight_;
};