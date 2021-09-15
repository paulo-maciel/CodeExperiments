#pragma once

#include <vulkan/vulkan.h>

class SwapChain;
class Device;
class DepthStencil;

#include <vector>
#include <string>
#include <memory>

#define NDEBUG 0

class GraphicsPipeline {
public:
  static std::vector<char> readFile(const std::string& filename);

  GraphicsPipeline(std::shared_ptr<Device> device, std::shared_ptr<SwapChain> swapChain, std::shared_ptr<DepthStencil> depthStencil);
  ~GraphicsPipeline();

  void create();
  void destroy();

  void createRenderPass();
  VkRenderPass getRenderPass() const;

  VkPipeline getPipeline() const;
  VkPipelineLayout getPipelineLayout() const;

  VkDescriptorSetLayout getDescriptorSetLayout() const;

private:
  void createDescriptorSetLayout();
  void createShaderStages();
  VkShaderModule createShaderModule(const std::vector<char>& code);
  void createPipeline();

private:
  uint8_t numStages_;
  VkDescriptorSetLayout descriptorSetLayout_;
  VkShaderModule vertShaderModule_;
  VkShaderModule fragShaderModule_;
  
  VkPipelineShaderStageCreateInfo *shaderStages_;

  std::shared_ptr<Device> device_;
  std::shared_ptr<DepthStencil> depthStencil_;
  // Swapchain for 2D extent and format.
  std::shared_ptr<SwapChain> swapChain_;

  VkRenderPass renderPass_;
  VkPipelineLayout pipelineLayout_;

  // The actual graphics pipeline.
  VkPipeline graphicsPipeline_;

};