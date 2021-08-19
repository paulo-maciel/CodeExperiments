#include <SyncObjects.h>
#include <Device.h>

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
    imageAvailableSemaphores_.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores_.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences_.resize(MAX_FRAMES_IN_FLIGHT);
    imagesInFlight_.resize(device_->getSwapChain()->getBufferCount(), VK_NULL_HANDLE);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(device_->getLogicalDevice(), &semaphoreInfo, nullptr, &imageAvailableSemaphores_[i]) != VK_SUCCESS ||
            vkCreateSemaphore(device_->getLogicalDevice(), &semaphoreInfo, nullptr, &renderFinishedSemaphores_[i]) != VK_SUCCESS ||
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
        vkDestroySemaphore(device_->getLogicalDevice(), renderFinishedSemaphores_[i], nullptr);
        vkDestroySemaphore(device_->getLogicalDevice(), imageAvailableSemaphores_[i], nullptr);
        vkDestroyFence(device_->getLogicalDevice(), inFlightFences_[i], nullptr);
    }
}

std::vector<VkSemaphore> &SyncObjects::getImageAvailableSemaphores() {
  return imageAvailableSemaphores_;
}

std::vector<VkSemaphore> &SyncObjects::getRenderFinishedSemaphores() {
  return renderFinishedSemaphores_;
}

std::vector<VkFence> &SyncObjects::getInflightFences() {
  return inFlightFences_;
}

std::vector<VkFence> &SyncObjects::getImagesInflight() {
  return imagesInFlight_;
}