#include <TextureImageSampler.h>

#include <Device.h>

#include <cstring>

using namespace std;

TextureImageSampler::TextureImageSampler(std::shared_ptr<Device> device)
  : device_(device) {
}

TextureImageSampler::~TextureImageSampler() {
}

bool TextureImageSampler::create() {
  VkPhysicalDeviceProperties properties{};
  vkGetPhysicalDeviceProperties(device_->getPhysicalDevice(), &properties);

  VkSamplerCreateInfo samplerInfo{};
  samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  samplerInfo.magFilter = VK_FILTER_LINEAR;
  samplerInfo.minFilter = VK_FILTER_LINEAR;
  samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.anisotropyEnable = VK_TRUE;
  samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
  samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
  samplerInfo.unnormalizedCoordinates = VK_FALSE;
  samplerInfo.compareEnable = VK_FALSE;
  samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
  samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

  if (vkCreateSampler(device_->getLogicalDevice(), &samplerInfo, nullptr, &textureSampler_) != VK_SUCCESS) {
    throw std::runtime_error("failed to create texture sampler!");
  }
}

void TextureImageSampler::destroy() {
  vkDestroySampler(device_->getLogicalDevice(), textureSampler_, nullptr);
}

VkSampler TextureImageSampler::getSampler() const {
  return textureSampler_;
}