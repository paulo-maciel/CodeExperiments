#include <DescriptorPool.h>

#include <Device.h>
#include <VertexBuffer.h>
#include <SwapChain.h>
#include <QueueSelector.h>
#include <GraphicsPipeline.h>
#include <UniformBuffer.h>
#include <TextureImage.h>
#include <TextureImageSampler.h>

#include <map>
#include <iostream>
#include <fstream>
#include <vector>
#include <set>

using namespace std;

DescriptorPool::DescriptorPool(std::shared_ptr<Device> device, 
                              std::shared_ptr<SwapChain> swapChain,
                              std::shared_ptr<GraphicsPipeline> graphicsPipeline)
: device_(device)
, swapChain_(swapChain)
, graphicsPipeline_(graphicsPipeline) {
}

DescriptorPool::~DescriptorPool() {
  cout << "DescriptorPool destructor called." << endl;
}

void DescriptorPool::destroy() {
  vkDestroyDescriptorPool(device_->getLogicalDevice(), descriptorPool_, nullptr);

  // No longer need the device.
  device_.reset();
}

void DescriptorPool::create() {
  // TODO: App specific.
  std::array<VkDescriptorPoolSize, 2> poolSizes{};
  poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  poolSizes[0].descriptorCount = static_cast<uint32_t>(swapChain_->getBufferCount());
  poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  poolSizes[1].descriptorCount = static_cast<uint32_t>(swapChain_->getBufferCount());

  VkDescriptorPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
  poolInfo.pPoolSizes = poolSizes.data();
  poolInfo.maxSets = static_cast<uint32_t>(swapChain_->getBufferCount());

  if (vkCreateDescriptorPool(device_->getLogicalDevice(), &poolInfo, nullptr, &descriptorPool_) != VK_SUCCESS) {
      throw std::runtime_error("failed to create descriptor pool!");
  }
}

VkDescriptorPool DescriptorPool::getDescriptorPool() const {
  return descriptorPool_;
}

void DescriptorPool::createDescriptorSets(std::shared_ptr<UniformBuffer> uniformBuffer,
                                          std::shared_ptr<TextureImage> textureImage,
                                          std::shared_ptr<TextureImageSampler> textureImageSampler) {
  std::vector<VkDescriptorSetLayout> layouts(swapChain_->getBufferCount(), graphicsPipeline_->getDescriptorSetLayout());

  VkDescriptorSetAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = descriptorPool_;
  allocInfo.descriptorSetCount = static_cast<uint32_t>(swapChain_->getBufferCount());
  allocInfo.pSetLayouts = layouts.data();

  descriptorSets_.resize(swapChain_->getBufferCount());
  if (vkAllocateDescriptorSets(device_->getLogicalDevice(), &allocInfo, descriptorSets_.data()) != VK_SUCCESS)   {
    throw std::runtime_error("failed to allocate descriptor sets!");
  }

  for (size_t i = 0; i < swapChain_->getBufferCount(); i++)   {
    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer = uniformBuffer->getUniformBuffers()[i];
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof(UniformBuffer::UniformBufferObject);

    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.sampler = textureImageSampler->getSampler();
    imageInfo.imageView = textureImage->getTextureImageView();

    std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

    descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[0].dstSet = descriptorSets_[i];
    descriptorWrites[0].dstBinding = 0;
    descriptorWrites[0].dstArrayElement = 0;
    descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrites[0].descriptorCount = 1;
    descriptorWrites[0].pBufferInfo = &bufferInfo;

    descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[1].dstSet = descriptorSets_[i];
    descriptorWrites[1].dstBinding = 1;
    descriptorWrites[1].dstArrayElement = 0;
    descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrites[1].descriptorCount = 1;
    descriptorWrites[1].pImageInfo = &imageInfo;

    vkUpdateDescriptorSets(device_->getLogicalDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
  }
}

std::vector<VkDescriptorSet> DescriptorPool::getDescriptorSets() const {
  return descriptorSets_;
}
