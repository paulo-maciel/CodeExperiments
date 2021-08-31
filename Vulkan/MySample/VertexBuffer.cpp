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

VertexBuffer::VertexBuffer(std::shared_ptr<Device> device, const std::vector<Vertex> &vertices)
  : device_(device)
  , vertices_(vertices) {
}

VertexBuffer::~VertexBuffer() {
}

bool VertexBuffer::create() {

  VkBufferCreateInfo bufferInfo{};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = sizeof(vertices_[0]) * vertices_.size();
  bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateBuffer(device_->getLogicalDevice(), &bufferInfo, nullptr, &vertexBuffer_) != VK_SUCCESS)
  {
    throw std::runtime_error("failed to create vertex buffer!");
  }

  VkMemoryRequirements memRequirements;
  vkGetBufferMemoryRequirements(device_->getLogicalDevice(), vertexBuffer_, &memRequirements);

  VkMemoryAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

  if (vkAllocateMemory(device_->getLogicalDevice(), &allocInfo, nullptr, &vertexBufferMemory_) != VK_SUCCESS)
  {
    throw std::runtime_error("failed to allocate vertex buffer memory!");
  }

  vkBindBufferMemory(device_->getLogicalDevice(), vertexBuffer_, vertexBufferMemory_, 0);

  // Note: It is also possible to specify the special value VK_WHOLE_SIZE to
  //       map all of the memory.
  void *data;
  vkMapMemory(device_->getLogicalDevice(), vertexBufferMemory_, 0, bufferInfo.size, 0, &data);
  memcpy(data, vertices_.data(), (size_t)bufferInfo.size);
  vkUnmapMemory(device_->getLogicalDevice(), vertexBufferMemory_);

  return true;
}

void VertexBuffer::destroy() {
  vkDestroyBuffer(device_->getLogicalDevice(), vertexBuffer_, nullptr);
  vkFreeMemory(device_->getLogicalDevice(), vertexBufferMemory_, nullptr);
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