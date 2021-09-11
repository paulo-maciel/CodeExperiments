#pragma once

#include <Buffer.h>

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

#include <vector>
#include <array>
#include <memory>

class Device;

class VertexBuffer : public Buffer {
public:
  struct Vertex;

  VertexBuffer(std::shared_ptr<Device> device, std::shared_ptr<CommandPool> commandPool, std::shared_ptr<QueueSelector> queueSelector, 
               const std::vector<Vertex> &vertices, const std::vector<uint16_t> indices);
  ~VertexBuffer();

  struct Vertex
  {
    glm::vec2 pos;
    glm::vec3 color;

    static VkVertexInputBindingDescription getBindingDescription();
    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions();
  };

  bool create(VkCommandPool commandPool, VkQueue graphicsQueue);
  void destroy();

  // Create a single index buffer
  void createIndexBuffer();

  // Copy one buffer (CPU/staging) to another GPU buffer.
  void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

  VkBuffer getVkBuffer() const;
  std::vector<VertexBuffer::Vertex> getVertices() const;

  VkBuffer getIndexBuffer() const;
  std::vector<uint16_t> getIndices() const;

private:
  uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

private:
  const std::vector<Vertex> vertices_;
  const std::vector<uint16_t> indices_;

  VkBuffer vertexBuffer_;
  VkDeviceMemory vertexBufferMemory_;

  VkBuffer indexBuffer_;
  VkDeviceMemory indexBufferMemory_;

  VkCommandPool commandPool_;
  VkQueue graphicsQueue_;
  };