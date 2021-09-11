#include <VertexBuffer.h>

#include <Device.h>

#include <cstring>

// static
VkVertexInputBindingDescription VertexBuffer::Vertex::getBindingDescription() {
  VkVertexInputBindingDescription bindingDescription{};
  bindingDescription.binding = 0;
  bindingDescription.stride = sizeof(Vertex);
  bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

  return bindingDescription;
}

// static
std::array<VkVertexInputAttributeDescription, 2> VertexBuffer::Vertex::getAttributeDescriptions() {

  std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

  attributeDescriptions[0].binding = 0;
  attributeDescriptions[0].location = 0;
  attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
  attributeDescriptions[0].offset = offsetof(Vertex, pos);

  attributeDescriptions[1].binding = 0;
  attributeDescriptions[1].location = 1;
  attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
  attributeDescriptions[1].offset = offsetof(Vertex, color);

  return attributeDescriptions;
}


VertexBuffer::VertexBuffer(std::shared_ptr<Device> device, std::shared_ptr<CommandPool> commandPool, std::shared_ptr<QueueSelector> queueSelector,
                           const std::vector<Vertex> &vertices, const std::vector<uint16_t> indices)
  : Buffer(device, commandPool, queueSelector)
  , vertices_(vertices)
  , indices_(indices) {
}

VertexBuffer::~VertexBuffer() {
}

bool VertexBuffer::create(VkCommandPool commandPool, VkQueue graphicsQueue)
{
  commandPool_ = commandPool;
  graphicsQueue_ = graphicsQueue;

  VkDeviceSize bufferSize = sizeof(vertices_[0]) * vertices_.size();

  // Create the CPU (e.g. staging buffer) to be copied to the GPU.
  VkBuffer stagingBuffer;
  VkDeviceMemory stagingBufferMemory;
  createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

  // Note: It is also possible to specify the special value VK_WHOLE_SIZE to
  //       map all of the memory.
  void *data;
  vkMapMemory(device_->getLogicalDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
  memcpy(data, vertices_.data(), (size_t)bufferSize);
  vkUnmapMemory(device_->getLogicalDevice(), stagingBufferMemory);

  // Create the GPU/high performance destination buffer
  createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer_, vertexBufferMemory_);

  copyBuffer(stagingBuffer, vertexBuffer_, bufferSize);

  // Staging buffer is no longer needed.
  vkDestroyBuffer(device_->getLogicalDevice(), stagingBuffer, nullptr);
  vkFreeMemory(device_->getLogicalDevice(), stagingBufferMemory, nullptr);

  // Create the index buffer.
  createIndexBuffer();

  return true;
}

void VertexBuffer::destroy() {
  vkDestroyBuffer(device_->getLogicalDevice(), vertexBuffer_, nullptr);
  vkFreeMemory(device_->getLogicalDevice(), vertexBufferMemory_, nullptr);

  vkDestroyBuffer(device_->getLogicalDevice(), indexBuffer_, nullptr);
  vkFreeMemory(device_->getLogicalDevice(), indexBufferMemory_, nullptr);
}

uint32_t VertexBuffer::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
  VkPhysicalDeviceMemoryProperties memProperties;
  vkGetPhysicalDeviceMemoryProperties(device_->getPhysicalDevice(), &memProperties);

  for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
    if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
      return i;
    }
  }

  throw std::runtime_error("failed to find suitable memory type!");
}

VkBuffer VertexBuffer::getVkBuffer() const {
  return vertexBuffer_;
}

std::vector<VertexBuffer::Vertex> VertexBuffer::getVertices() const {
  return vertices_;
}

VkBuffer VertexBuffer::getIndexBuffer() const {
  return indexBuffer_;
}

std::vector<uint16_t> VertexBuffer::getIndices() const {
  return indices_;
}

void VertexBuffer::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory)
{
  VkBufferCreateInfo bufferInfo{};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = size;
  bufferInfo.usage = usage;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateBuffer(device_->getLogicalDevice(), &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
    throw std::runtime_error("failed to create buffer!");
  }

  VkMemoryRequirements memRequirements;
  vkGetBufferMemoryRequirements(device_->getLogicalDevice(), buffer, &memRequirements);

  VkMemoryAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

  if (vkAllocateMemory(device_->getLogicalDevice(), &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
  {
    throw std::runtime_error("failed to allocate buffer memory!");
  }

  vkBindBufferMemory(device_->getLogicalDevice(), buffer, bufferMemory, 0);
}

void VertexBuffer::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandPool = commandPool_;
  allocInfo.commandBufferCount = 1;

  VkCommandBuffer commandBuffer;
  vkAllocateCommandBuffers(device_->getLogicalDevice(), &allocInfo, &commandBuffer);

  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  vkBeginCommandBuffer(commandBuffer, &beginInfo);

  VkBufferCopy copyRegion{};
  copyRegion.size = size;
  vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

  vkEndCommandBuffer(commandBuffer);

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffer;

  vkQueueSubmit(graphicsQueue_, 1, &submitInfo, VK_NULL_HANDLE);
  vkQueueWaitIdle(graphicsQueue_);

  vkFreeCommandBuffers(device_->getLogicalDevice(), commandPool_, 1, &commandBuffer);
}

void VertexBuffer::createIndexBuffer()
{
  VkDeviceSize bufferSize = sizeof(indices_[0]) * indices_.size();

  VkBuffer stagingBuffer;
  VkDeviceMemory stagingBufferMemory;
  createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

  void *data;
  vkMapMemory(device_->getLogicalDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
  memcpy(data, indices_.data(), (size_t)bufferSize);
  vkUnmapMemory(device_->getLogicalDevice(), stagingBufferMemory);

  createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer_, indexBufferMemory_);

  copyBuffer(stagingBuffer, indexBuffer_, bufferSize);

  vkDestroyBuffer(device_->getLogicalDevice(), stagingBuffer, nullptr);
  vkFreeMemory(device_->getLogicalDevice(), stagingBufferMemory, nullptr);
}