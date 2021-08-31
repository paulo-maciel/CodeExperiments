#pragma once

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

#include <vector>
#include <array>
#include <memory>

class Device;

class VertexBuffer
{
public:
  struct Vertex;

  VertexBuffer(std::shared_ptr<Device> device, const std::vector<Vertex>& vertices);
  ~VertexBuffer();

  struct Vertex
  {
    glm::vec2 pos;
    glm::vec3 color;

    static VkVertexInputBindingDescription getBindingDescription();
    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions();
  };

  bool create();
  void destroy();

  VkBuffer getVkBuffer() const;
  std::vector<VertexBuffer::Vertex> getVertices() const;

private:
  uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

private:
  const std::vector<Vertex> vertices_;

  std::shared_ptr<Device> device_;
  VkBuffer vertexBuffer_;
  VkDeviceMemory vertexBufferMemory_;
};