#include <DepthStencil.h>

#include <Device.h>

#include <cstring>

using namespace std;

DepthStencil::DepthStencil() {
}

DepthStencil::~DepthStencil() {
}

bool DepthStencil::create(std::shared_ptr<Device> device, uint32_t width, uint32_t height) {
  VkFormat depthFormat = findDepthFormat();

  Image::create(device, width, height, depthFormat, 
                VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, 
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
                depthImage_, depthImageMemory_);
  // TODO depthImageView = createImageView(depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
}

void DepthStencil::destroy() {
  //vkDestroySampler(device_->getLogicalDevice(), textureSampler_, nullptr);
}

VkFormat DepthStencil::findDepthFormat() {
  VkFormat format;
  return format;
}