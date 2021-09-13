#pragma once

#include <vulkan/vulkan.h>


#include <vector>
#include <string>
#include <memory>

#define NDEBUG 0

class Device;
class VertexBuffer;
class SwapChain;
class GraphicsPipeline;
class UniformBuffer;
class TextureImage;
class TextureImageSampler;

class DescriptorPool {
public:
  DescriptorPool(std::shared_ptr<Device> device,
              std::shared_ptr<SwapChain> swapChain,
              std::shared_ptr<GraphicsPipeline> graphicsPipeline);
  ~DescriptorPool();

  void create();
  void destroy();

  void createDescriptorSets(std::shared_ptr<UniformBuffer> uniformBuffer,
                            std::shared_ptr<TextureImage> textureImage,
                            std::shared_ptr<TextureImageSampler> textureImageSampler);
  std::vector<VkDescriptorSet> getDescriptorSets() const;

  VkDescriptorPool getDescriptorPool() const;

private:
  std::shared_ptr<Device> device_;
  std::shared_ptr<SwapChain> swapChain_;
  std::shared_ptr<GraphicsPipeline> graphicsPipeline_;

  VkDescriptorPool descriptorPool_;
  std::vector<VkDescriptorSet> descriptorSets_;
};