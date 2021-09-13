#pragma once

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

#include <memory>

class Device;

class TextureImageSampler {
public:

  TextureImageSampler(std::shared_ptr<Device> device);
  ~TextureImageSampler();

  bool create();
  void destroy();

  VkSampler getSampler() const;
           
private:
  std::shared_ptr<Device> device_;
  VkSampler textureSampler_;
};