#include <DescriptorPool.h>

#include <Device.h>
#include <VertexBuffer.h>
#include <SwapChain.h>
#include <QueueSelector.h>
#include <GraphicsPipeline.h>
#include <UniformBuffer.h>

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

void DescriptorPool::createDescriptorSets(std::shared_ptr<UniformBuffer> uniformBuffer) {
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

    VkWriteDescriptorSet descriptorWrite{};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = descriptorSets_[i];
    descriptorWrite.dstBinding = 0;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pBufferInfo = &bufferInfo;

    vkUpdateDescriptorSets(device_->getLogicalDevice(), 1, &descriptorWrite, 0, nullptr);
  }
}

std::vector<VkDescriptorSet> DescriptorPool::getDescriptorSets() const {
  return descriptorSets_;
}
