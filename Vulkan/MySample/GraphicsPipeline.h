#pragma once

#include <vulkan/vulkan.h>

#include <vector>
#include <string>

#define NDEBUG 0

class GraphicsPipeline {
public:
  static std::vector<char> readFile(const std::string& filename);
  
  GraphicsPipeline(VkDevice device, VkExtent2D extent2D);
  ~GraphicsPipeline();

  void create();
  void destroy();

private:
  VkShaderModule createShaderModule(const std::vector<char>& code);
  void configFixedFunction();

private:
  VkDevice device_;
  // Swapchain 2D extent.
  VkExtent2D extent2D_;
  VkPipelineLayout pipelineLayout_;

};