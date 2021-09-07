#include <UniformBuffer.h>

#include <Device.h>
#include <SwapChain.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cstring>
#include <chrono>
#include <iostream>

using namespace std;

UniformBuffer::UniformBuffer(std::shared_ptr<Device> device, std::shared_ptr<SwapChain> swapChain)
  : Buffer(device)
  , swapChain_(swapChain) {
}

UniformBuffer::~UniformBuffer() {
}

std::vector<VkBuffer> UniformBuffer::getUniformBuffers() const {
  return uniformBuffers_;
}

bool UniformBuffer::create() {
  VkDeviceSize bufferSize = sizeof(UniformBuffer::UniformBufferObject);

  uniformBuffers_.resize(swapChain_->getBufferCount());
  uniformBuffersMemory_.resize(swapChain_->getBufferCount());

  // Allocate buffer memory
  for (size_t i = 0; i < swapChain_->getBufferCount(); i++) {
    Buffer::create(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers_[i], uniformBuffersMemory_[i]);
  }
}

void UniformBuffer::destroy() {
  for (size_t i = 0; i < swapChain_->getBufferCount(); i++) {
    vkDestroyBuffer(device_->getLogicalDevice(), uniformBuffers_[i], nullptr);
    vkFreeMemory(device_->getLogicalDevice(), uniformBuffersMemory_[i], nullptr);
  }
}

bool UniformBuffer::update(uint32_t currentImage) {
  static auto startTime = std::chrono::high_resolution_clock::now();

  auto currentTime = std::chrono::high_resolution_clock::now();
  float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

  // Note: In Vulkan, x to right, y is down, and z is inside the screen.  A right handed CS.
  UniformBufferObject ubo{};
  ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
  ubo.view = glm::lookAt(glm::vec3(0.0f, 0.0f, -3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
  ubo.proj = glm::perspective(glm::radians(45.0f), swapChain_->getExtent2D().width / (float)swapChain_->getExtent2D().height, 0.1f, 10.0f);

  // Note: GLM was originally designed for OpenGL, where the Y coordinate of the clip
  // coordinates is inverted. The easiest way to compensate for that is to flip the
  // sign on the scaling factor of the Y axis in the projection matrix. If you don’t
  // do this, then the image will be rendered upside down. 
  ubo.proj[1][1] *= -1;

  void *data;
  vkMapMemory(device_->getLogicalDevice(), uniformBuffersMemory_[currentImage], 0, sizeof(ubo), 0, &data);
  memcpy(data, &ubo, sizeof(ubo));
  vkUnmapMemory(device_->getLogicalDevice(), uniformBuffersMemory_[currentImage]);

  return true;
}