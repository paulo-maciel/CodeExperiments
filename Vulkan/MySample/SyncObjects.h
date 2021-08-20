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

  // Two semaphores are needed:
  // One semaphore to signal that an image has been acquired and is
  // ready for rendering (imageAcquiredSemaphores_)
  // The other to signal that rendering has finished and
  // presentation can happen (doneRenderingSemaphores_). 
  std::vector<VkSemaphore> imageAcquiredSemaphores_;
  std::vector<VkSemaphore> doneRenderingSemaphores_;

  std::vector<VkFence> inFlightFences_;
  std::vector<VkFence> imagesInFlight_;
};