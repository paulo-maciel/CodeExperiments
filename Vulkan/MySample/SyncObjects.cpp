#include <SyncObjects.h>
#include <Device.h>
#include <SwapChain.h>

#include <map>
#include <iostream>
#include <vector>
#include <set>

using namespace std;

const int MAX_FRAMES_IN_FLIGHT = 2;

SyncObjects::SyncObjects(std::shared_ptr<Device> device) 
  : device_(device) {
}

SyncObjects::~SyncObjects() {
    cout << "SyncObjects destructor called." << endl;
}

bool SyncObjects::create() {
    imageAcquiredSemaphores_.resize(MAX_FRAMES_IN_FLIGHT);
    doneRenderingSemaphores_.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences_.resize(MAX_FRAMES_IN_FLIGHT);
    imagesInFlight_.resize(device_->getSwapChain()->getBufferCount(), VK_NULL_HANDLE);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

    // Need to create the fence in a signaled state otherwise when
    // drawFrame gets called for the first time,it will wait forever.
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(device_->getLogicalDevice(), &semaphoreInfo, nullptr, &imageAcquiredSemaphores_[i]) != VK_SUCCESS ||
            vkCreateSemaphore(device_->getLogicalDevice(), &semaphoreInfo, nullptr, &doneRenderingSemaphores_[i]) != VK_SUCCESS ||
            vkCreateFence(device_->getLogicalDevice(), &fenceInfo, nullptr, &inFlightFences_[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create synchronization objects for a frame!");
            return false;
        }
    }

    cout << "Sync objects created." << endl;
    return true;
}

bool SyncObjects::destroy() {
  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
      vkDestroySemaphore(device_->getLogicalDevice(), doneRenderingSemaphores_[i], nullptr);
      vkDestroySemaphore(device_->getLogicalDevice(), imageAcquiredSemaphores_[i], nullptr);
      vkDestroyFence(device_->getLogicalDevice(), inFlightFences_[i], nullptr);
  }
  return true;
}

std::vector<VkSemaphore> &SyncObjects::getImageAvailableSemaphores() {
  return imageAcquiredSemaphores_;
}

std::vector<VkSemaphore> &SyncObjects::getRenderFinishedSemaphores() {
  return doneRenderingSemaphores_;
}

std::vector<VkFence> &SyncObjects::getInflightFences() {
  return inFlightFences_;
}

std::vector<VkFence> &SyncObjects::getImagesInflight() {
  return imagesInFlight_;
}