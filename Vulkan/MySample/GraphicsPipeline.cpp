#include <GraphicsPipeline.h>

#include <VertexBuffer.h>

#include <map>
#include <iostream>
#include <fstream>
#include <vector>
#include <set>

using namespace std;
using Vertex = VertexBuffer::Vertex;

GraphicsPipeline::GraphicsPipeline(VkDevice device, std::shared_ptr<SwapChain> swapChain)
: numStages_(2)
, device_(device)
, swapChain_(swapChain) {
}

GraphicsPipeline::~GraphicsPipeline() {
    cout << "GraphicsPipeline destructor called." << endl;
}

void GraphicsPipeline::destroy() {
    cout << "Destroying the render pass." << endl;
    vkDestroyRenderPass(device_, renderPass_, nullptr);

    cout << "Destroying the pipeline layout." << endl;
    vkDestroyPipelineLayout(device_, pipelineLayout_, nullptr);

    cout << "Destroying the pipeline." << endl;
    vkDestroyPipeline(device_, graphicsPipeline_, nullptr); 

    cout << "Destroying descriptor set layout." << endl;
    vkDestroyDescriptorSetLayout(device_, descriptorSetLayout_, nullptr);
}

void GraphicsPipeline::createRenderPass() {
    VkAttachmentDescription colorAttachment{};

    // Note: The format of the color attachment should match the format of 
    // the swap chain images.
    colorAttachment.format = swapChain_->getFormat().format;

    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;

    // The loadOp and storeOp determine what to do with the data in the attachment
    //  before rendering and after rendering. We have the following choices for loadOp:
    //• VK_ATTACHMENT_LOAD_OP_LOAD: Preserve the existing contents of the attachment
    //• VK_ATTACHMENT_LOAD_OP_CLEAR: Clear the values to a constant at the start
    //• VK_ATTACHMENT_LOAD_OP_DONT_CARE: Existing contents are undefined;
    // we don’t care about them.
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;

    // There are two possibilities for the storeOp:
    //• VK_ATTACHMENT_STORE_OP_STORE: Rendered contents will be stored in
    // memory and can be read later
    //• VK_ATTACHMENT_STORE_OP_DONT_CARE: Contents of the framebuffer will
    // be undefined after the rendering operation
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    // Common layouts: 
    // VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL: Images used as color attachment
    // VK_IMAGE_LAYOUT_PRESENT_SRC_KHR: Images to be presented in the swap chain
    // VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL: Images to be used as destination for 
    // a memory copy operation
    // Note: The initialLayout specifies which layout the image will have before the 
    // render pass begins. The finalLayout specifies the layout to automatically 
    // transition to when the render pass finishes.
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    // Note: A single render pass can consist of multiple subpasses. Subpasses are subsequent
    // rendering operations that depend on the contents of framebuffers in previous
    // passes, for example a sequence of post-processing effects that are applied one
    // after another.
    VkAttachmentReference colorAttachmentRef{};
    // Index is 0 here since we only have one color attachment in the attachment 
    // descriptions array (i.e. VkAttachmentDescription)
    // Important: The index of the attachment in this array is directly referenced 
    // from the fragment shader with the layout(location = 0)out vec4 outColor directive!
    colorAttachmentRef.attachment = 0;  
    // intend to use the attachment to function as color buffer
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    // Description of the subpass.
    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;  // as opposed to COMPUTE

    subpass.colorAttachmentCount = 1;

    // Note: The following other types of attachments can be referenced by a subpass:
    // • pInputAttachments: Attachments that are read from a shader
    // • pResolveAttachments: Attachments used for multisampling color attachments
    // • pDepthStencilAttachment: Attachment for depth and stencil data
    // • pPreserveAttachments: Attachments that are not used by this subpass,
    //   but for which the data must be preserved.
    subpass.pColorAttachments = &colorAttachmentRef;

    VkSubpassDependency dependency{};
    // The special value VK_SUBPASS_EXTERNAL refers to the implicit subpass before or 
    // after the render pass depending on whether it is specified in srcSubpass
    // or dstSubpass.
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;

    // The next two fields specify the operations to wait on and the stages in which
    // these operations occur. We need to wait for the swap chain to finish reading
    // from the image before we can access it. This can be accomplished by waiting
    // on the color attachment output stage itself.
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    // Now, create the renderpass.
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(device_, &renderPassInfo, nullptr, &renderPass_) != VK_SUCCESS) {
        throw std::runtime_error("failed to create render pass!");
    }

    cout << "Renderpass created." << endl << endl;
}

VkRenderPass GraphicsPipeline::getRenderPass() const {
  return renderPass_;
}

VkPipeline GraphicsPipeline::getPipeline() const {
  return graphicsPipeline_;
}

VkPipelineLayout GraphicsPipeline::getPipelineLayout() const {
  return pipelineLayout_;
}

VkDescriptorSetLayout GraphicsPipeline::getDescriptorSetLayout() const {
  return descriptorSetLayout_;
}

// In order to create the pipeline, we need:
// • Shader stages: the shader modules that define the functionality of the
// programmable stages of the graphics pipeline
// • Fixed-function state: all of the structures that define the fixed-function
// stages of the pipeline, like input assembly, rasterizer, viewport and color
// blending
// • Pipeline layout: the uniform and push values referenced by the shader
// that can be updated at draw time
// • Render pass: the attachments referenced by the pipeline stages and their
// usage
void GraphicsPipeline::create() {
    // First create a render pass.
    createRenderPass();

    // Create the descriptor set layout for the uniform buffers
    createDescriptorSetLayout();

    // Create the pipeline shader stages.
    createShaderStages();
    cout << "Created the shader stages." << endl;

    // Configure what used to be the fixed function portion of the pipeline
    createPipeline();

    // Shader modules no longer needed.
    vkDestroyShaderModule(device_, fragShaderModule_, nullptr);
    vkDestroyShaderModule(device_, vertShaderModule_, nullptr);
}

void GraphicsPipeline::createDescriptorSetLayout() {

  VkDescriptorSetLayoutBinding uboLayoutBinding{};
  uboLayoutBinding.binding = 0;
  uboLayoutBinding.descriptorCount = 1;
  uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  uboLayoutBinding.pImmutableSamplers = nullptr;
  uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

  VkDescriptorSetLayoutBinding samplerLayoutBinding{};
  samplerLayoutBinding.binding = 1;
  samplerLayoutBinding.descriptorCount = 1;
  samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  samplerLayoutBinding.pImmutableSamplers = nullptr;
  samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

  std::array<VkDescriptorSetLayoutBinding, 2> bindings = {uboLayoutBinding, samplerLayoutBinding};
  VkDescriptorSetLayoutCreateInfo layoutInfo{};
  layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
  layoutInfo.pBindings = bindings.data();

  if (vkCreateDescriptorSetLayout(device_, &layoutInfo, nullptr, &descriptorSetLayout_) != VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor set layout!");
  }
}

VkShaderModule GraphicsPipeline::createShaderModule(const std::vector<char>& code) {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(device_, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module!");
    }

    return shaderModule;
}

void GraphicsPipeline::createShaderStages() {
    auto vertShaderCode = readFile("shaders/mysamplevs.spv");
    auto fragShaderCode = readFile("shaders/mysamplefs.spv");

    vertShaderModule_ = createShaderModule(vertShaderCode);
    fragShaderModule_ = createShaderModule(fragShaderCode);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule_;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule_;
    fragShaderStageInfo.pName = "main";

    // List of shader stages.
    shaderStages_ = new VkPipelineShaderStageCreateInfo[numStages_];
    shaderStages_[0] = vertShaderStageInfo;
    shaderStages_[1] = fragShaderStageInfo;

    // TODO: Can the VkShaderModule (s) be deleted once they've been copied to 
    // the pipeline shader stage create info?
}

void GraphicsPipeline::createPipeline() {
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    auto bindingDescription = Vertex::getBindingDescription();
    auto attributeDescriptions = Vertex::getAttributeDescriptions();
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount =
        static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    // Describes the  kind of geometry to draw from the vertices and if primitive restart
    // should be enabled.
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    // If primitiveRestartEnable member is set to VK_TRUE, then it’s possible to break up
    // lines and triangles in the _STRIP topology modes by using a special index of
    // 0xFFFF or 0xFFFFFFFF.
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    auto extent2D = swapChain_->getExtent2D();
    viewport.width = (float) extent2D.width;
    viewport.height = (float) extent2D.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = extent2D;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;

    // If rasterizerDiscardEnable is set to VK_TRUE, then geometry never passes
    // through the rasterizer stage. This basically disables any output to the frame-
    // buffer.
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;

    // The rasterizer can alter the depth values by adding a constant value or biasing
    // them based on a fragment’s slope. This is sometimes used for shadow mapping.
    rasterizer.depthBiasEnable = VK_FALSE;
    // rasterizer.depthBiasConstantFactor = 0.0f; // Optional
    // rasterizer.depthBiasClamp = 0.0f; // Optional
    // rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    // Create depth and stencil here if needed.
    // VkPipelineDepthStencilStateCreateInfo.


    // 2 way of doing Blending:
    // • Mix the old and new value to produce a final color
    // • Combine the old and new value using a bitwise operation

    // There are two types of structs to configure color blending. The first struct,
    // VkPipelineColorBlendAttachmentState contains the configuration per attached 
    // framebuffer and the second struct, VkPipelineColorBlendStateCreateInfo
    // contains the global color blending settings. In our case we only have one framebuffer.
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};

    // Note that: finalColor = finalColor & colorWriteMask
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    // colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    // colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    // colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
    // colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    // colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    // colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional


    // The VkPipelineColorBlendStateCreateInfo structure references the array of structures 
    // for all of the framebuffers and allows us to set blend constants that we can use as 
    // blend factors in the aforementioned calculations.
    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;

    // To use the second blend method, set colorBlending.logicOpEnable = VK_TRUE and
    // the appropriate logicOp.
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;


    // TODO: Config dynamic states such as below.  These don't require that the pipeline be
    // recreated:
    // VkDynamicState dynamicStates[] = {
    //     VK_DYNAMIC_STATE_VIEWPORT,
    //     VK_DYNAMIC_STATE_LINE_WIDTH
    // };

    // VkPipelineDynamicStateCreateInfo dynamicState{};
    // dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    // dynamicState.dynamicStateCount = 2;
    // dynamicState.pDynamicStates = dynamicStates;

    // TODO: Create uniform values for the shaders dugin pipeline creation below
    // when creating a VkPipelineLayout object.
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout_;
    //pipelineLayoutInfo.pushConstantRangeCount = 0;

    if (vkCreatePipelineLayout(device_, &pipelineLayoutInfo, nullptr, &pipelineLayout_) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }
    cout << "Created the pipeline layout." << endl;

    // Create the pipeline
    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = numStages_;
    pipelineInfo.pStages = shaderStages_;

    // From fixed function config
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.layout = pipelineLayout_;

    // From render pass.
    pipelineInfo.renderPass = renderPass_;

    // Note: subpass is the index of the subpass in the render pass 
    //       where this pipeline will be used.  Since there is only one
    //       index is 0.
    pipelineInfo.subpass = 0;

    // Note: basePipelineHandle and basePipelineIndex. Vulkan allows you to create a new graphics pipeline by
    // deriving from an existing pipeline. The idea of pipeline derivatives is that
    // it is less expensive to set up pipelines when they have much functionality in
    // common with an existing pipeline and switching between pipelines from the
    // same parent can also be done quicker. You can either specify the handle of an
    // existing pipeline with basePipelineHandle or reference another pipeline that
    // is about to be created by index with basePipelineIndex. Right now there is
    // only a single pipeline, so we’ll simply specify a null handle and an invalid index.
    // These values are only used if the VK_PIPELINE_CREATE_DERIVATIVE_BIT flag
    // is also specified in the flags field of VkGraphicsPipelineCreateInfo.
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    //pipelineInfo.basePipelineIndex = -1; // Optional


    // Note: The vkCreateGraphicsPipelines function actually has more parameters than
    // the usual object creation functions in Vulkan. It is designed to take multiple
    // VkGraphicsPipelineCreateInfo objects and create multiple VkPipeline ob-
    // jects in a single call.
    if (vkCreateGraphicsPipelines(device_, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline_) != VK_SUCCESS) {
        throw std::runtime_error("failed to create graphics pipeline!");
    }

    cout << "Created the pipeline!" << endl;
}

//static 
std::vector<char> GraphicsPipeline::readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    cout << "Read shader file: " << filename << endl;

    return buffer;
}