#pragma once

#include <Buffer.h>

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

#include <string>

class Device;
class CommandPool;
class QueueSelector;

class TextureImage : public Buffer {
public:

  TextureImage(std::shared_ptr<Device> device, std::shared_ptr<CommandPool> commandPool, std::shared_ptr<QueueSelector> queueSelector);
  ~TextureImage();

  bool create(const std::string &filename);
  void destroy();

private:
  void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling,
                   VkImageUsageFlags usage, VkMemoryPropertyFlags properties, 
                   VkImage &image, VkDeviceMemory &imageMemory);

  void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

  void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

  VkCommandBuffer beginSingleTimeCommands();
  void endSingleTimeCommands(VkCommandBuffer commandBuffer);
                  
private: 

  std::shared_ptr<CommandPool> commandPool_;
  std::shared_ptr<QueueSelector> queueSelector_;

  VkImage textureImage_;
  VkDeviceMemory textureImageMemory_;
};