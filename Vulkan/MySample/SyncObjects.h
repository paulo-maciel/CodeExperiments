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

  // If MAX_FRAMES_IN_FLIGHT is higher than the number of swap chain images
  // or vkAcquireNextImageKHR returns images out-of-order then it’s possible that
  // we may start rendering to a swap chain image that is already in flight. To avoid
  // this, we need to track for each swap chain image if a frame in flight is currently
  // using it. This mapping will refer to frames in flight by their fences so we’ll
  // immediately have a synchronization object to wait on before a new frame can
  // use that image.
  std::vector<VkFence> imagesInFlight_;
  std::vector<VkFence> inFlightFences_;

};