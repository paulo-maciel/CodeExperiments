#include <CommandPool.h>

#include <Device.h>

#include <map>
#include <iostream>
#include <fstream>
#include <vector>
#include <set>

using namespace std;

CommandPool::CommandPool(std::shared_ptr<Device> device)
: device_(device) {
}

CommandPool::~CommandPool() {
    cout << "CommandPool destructor called." << endl;
}

void CommandPool::create() {
    // TODO: fix below
    QueueSelector::QueueFamilyIndices queueFamilyIndices = device_->getQueueSelector()->findFamilies(device_->getPhysicalDevice());

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    if (vkCreateCommandPool(device_->getLogicalDevice(), &poolInfo, nullptr, &commandPool_) != VK_SUCCESS) {
        throw std::runtime_error("failed to create command pool!");
    }
}

void CommandPool::createCommandBuffers() {
    commandBuffers_.resize(device_->getSwapChain()->getFrameBuffers().size());

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool_;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t) commandBuffers_.size();

    if (vkAllocateCommandBuffers(device_->getLogicalDevice(), &allocInfo, commandBuffers_.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }

    for (size_t i = 0; i < commandBuffers_.size(); i++) {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffers_[i], &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = device_->getGraphicsPipeline()->getRenderPass();
        renderPassInfo.framebuffer = device_->getSwapChain()->getFrameBuffers()[i];
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = device_->getSwapChain()->getExtent2D();

        VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(commandBuffers_[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

            vkCmdBindPipeline(commandBuffers_[i], VK_PIPELINE_BIND_POINT_GRAPHICS, device_->getGraphicsPipeline()->getPipeline());

            vkCmdDraw(commandBuffers_[i], 3, 1, 0, 0);

        vkCmdEndRenderPass(commandBuffers_[i]);

        if (vkEndCommandBuffer(commandBuffers_[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }
    }
}