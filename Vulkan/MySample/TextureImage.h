#pragma once

#include <Buffer.h>
#include <Image.h>

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

#include <string>

class Device;


class TextureImage : public Buffer, public Image {
public:

  TextureImage(std::shared_ptr<Device> device, std::shared_ptr<CommandPool> commandPool, std::shared_ptr<QueueSelector> queueSelector);
  ~TextureImage();

  bool create(const std::string &filename, VkFormat imageViewFormat);
  void destroy();

  void createImageView(VkFormat imageViewFormat);

  VkImage getTextureImage() const;
  VkImageView getTextureImageView() const;

private:
  void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
  void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
                  
private: 
  VkImage textureImage_;
  VkImageView textureImageView_;
  VkDeviceMemory textureImageMemory_;
};