#pragma once

#include <vulkan/vulkan.h>

#include <SwapChain.h>

#include <vector>
#include <string>
#include <memory>

#define NDEBUG 0

class GraphicsPipeline {
public:
  static std::vector<char> readFile(const std::string& filename);
  
  GraphicsPipeline(VkDevice device, std::shared_ptr<SwapChain> swapChain);
  ~GraphicsPipeline();

  void create();
  void destroy();

  void createRenderPass();

private:
  void createShaderStages();
  VkShaderModule createShaderModule(const std::vector<char>& code);
  void createPipeline();

private:
  uint8_t numStages_;
  VkShaderModule vertShaderModule_;
  VkShaderModule fragShaderModule_;
  
  VkPipelineShaderStageCreateInfo *shaderStages_;

  VkDevice device_;

  // Swapchain for 2D extent and format.
  std::shared_ptr<SwapChain> swapChain_;

  VkRenderPass renderPass_;
  VkPipelineLayout pipelineLayout_;

  // The actual graphics pipeline.
  VkPipeline graphicsPipeline_;

};