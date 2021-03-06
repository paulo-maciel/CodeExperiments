#include <SwapChain.h>

#include <Device.h>

#include <map>
#include <iostream>
#include <vector>
#include <set>

using namespace std;

SwapChain::SwapChain(VkSurfaceKHR vkSurface)
  : vkSurface_(vkSurface) {
}

SwapChain::~SwapChain() {
    cout << "SwapChain destructor called." << endl;
}

VkSurfaceFormatKHR SwapChain::selectSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats) const {

  // For now, select 3 component sRGB BGRA
  for (const auto& format : formats) {
    if (format.format == VK_FORMAT_B8G8R8A8_SRGB &&
        format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
        return format;
    }
  }

  // Could not find the ideal format, just return the one we found.
  return formats[0];
}

VkPresentModeKHR SwapChain::selectSwapPresentMode(const std::vector<VkPresentModeKHR>& presentModes) const {
    for (const auto& presentMode : presentModes) {
        if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return presentMode;
        }
    }

    // VK_PRESENT_MODE_FIFO_KHR
    return presentModes[0];
}

VkExtent2D SwapChain::selectSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) const {
    if (capabilities.currentExtent.width != UINT32_MAX) {
        return capabilities.currentExtent;
    } else {
        VkExtent2D actualExtent = {800, 600};
        actualExtent.width = std::max(capabilities.minImageExtent.width, 
                                      std::min(capabilities.maxImageExtent.width, actualExtent.width));
        actualExtent.height = std::max(capabilities.minImageExtent.height,
                                       std::min(capabilities.maxImageExtent.height, actualExtent.height));

        return actualExtent;
    }
} 


SwapChain::Details SwapChain::getDeviceDetails(VkPhysicalDevice device) const {
    Details details;

    // Query basic surface capabilities.
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, vkSurface_, &details.capabilities);

    // Query the supported surface formats.
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, vkSurface_, &formatCount, nullptr);

    if (formatCount != 0) {
        cout << "Swap chain has: " << formatCount << " formats." << endl;
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, vkSurface_, &formatCount, details.formats.data());
    }

    // Query the supported presentation modes.
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, vkSurface_, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        cout << "Swap chain has: " << presentModeCount << " present modes." << endl;
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, vkSurface_, &presentModeCount, details.presentModes.data());
    }

    return details;
}

VkExtent2D SwapChain::getExtent2D() const {
  return extent2D_;
}

VkSurfaceFormatKHR SwapChain::getFormat() const {
  return format_;
}

std::vector<VkImageView> SwapChain::getImagesView() const {
  return imagesView_;
}

std::vector<VkFramebuffer> SwapChain::getFrameBuffers() const {
  return frameBuffers_;
}

void SwapChain::createFrameBuffers(VkRenderPass renderPass, VkImageView depthImageView) {
    frameBuffers_.resize(imagesView_.size());

    for (size_t i = 0; i < imagesView_.size(); i++) {
      std::array<VkImageView, 2> attachments = {
        imagesView_[i],
        depthImageView
      };

      VkFramebufferCreateInfo framebufferInfo{};
      framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
      framebufferInfo.renderPass = renderPass;
      framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
      framebufferInfo.pAttachments = attachments.data();
      framebufferInfo.width = extent2D_.width;
      framebufferInfo.height = extent2D_.height;
      framebufferInfo.layers = 1;

      if (vkCreateFramebuffer(device_, &framebufferInfo, nullptr, &frameBuffers_[i]) != VK_SUCCESS)
      {
        throw std::runtime_error("failed to create framebuffer!");
        }
    }
}

uint32_t SwapChain::getBufferCount() const {
  return bufferCount_;
}

bool SwapChain::create(std::shared_ptr<Device> device, QueueSelector::QueueFamilyIndices familyIndexes) {
    auto physicalDevice = device->getPhysicalDevice();
    device_ = device->getLogicalDevice();
    SwapChain::Details swapChainDetails = getDeviceDetails(physicalDevice);

    const std::vector<VkSurfaceFormatKHR>& formats = swapChainDetails.formats;
    format_ = selectSwapSurfaceFormat(formats);
    const std::vector<VkPresentModeKHR>& presentModes = swapChainDetails.presentModes;
    presentMode_ = selectSwapPresentMode(presentModes);
    extent2D_ = selectSwapExtent(swapChainDetails.capabilities);

    // Decide how many images to have in the swap chain.  To avoid waiting on the driver, 
    // request at least 1 more than the minimum.
    bufferCount_ = swapChainDetails.capabilities.minImageCount + 1;

    // Make sure to not exceed the maximum number of images.  Note that 0 means 
    // that there is no maximum:
    if (swapChainDetails.capabilities.maxImageCount > 0 && bufferCount_ > swapChainDetails.capabilities.maxImageCount) {
        bufferCount_ = swapChainDetails.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = vkSurface_;
    createInfo.minImageCount = bufferCount_;
    createInfo.imageFormat = format_.format;
    createInfo.imageColorSpace = format_.colorSpace;
    createInfo.imageExtent = extent2D_;
    createInfo.imageArrayLayers = 1;   // greater than one only for stereoscopic 3D applications
    // Render directly to the images by specifying VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
    // Use VK_IMAGE_USAGE_TRANSFER_DST_BIT instead if want to render to a separate image first 
    // to perform operations like post-processing and use a memory operation to transfer the 
    // rendered image to a swap chain image.
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; 


    // Note: VK_SHARING_MODE_EXCLUSIVE: An image is owned by one queue family
    //       at a time and ownership must be explicitly transfered before using it in
    //       another queue family. This option offers the best performance.
    //       VK_SHARING_MODE_CONCURRENT: Images can be used across multiple queue
    //       families without explicit ownership transfers.
    uint32_t queueFamilyIndices[] = {familyIndexes.graphicsFamily.value(),
                                     familyIndexes.presentFamily.value()};

    if (familyIndexes.graphicsFamily != familyIndexes.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {  // Most hardware
        cout << "Graphics and PresentMode queues are the same." << endl;
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0; // Optional
        createInfo.pQueueFamilyIndices = nullptr; // Optional
    }

    // To not apply any transformation to the images in the swap chain, just select the current transform.
    createInfo.preTransform = swapChainDetails.capabilities.currentTransform;

    // Ignore the alpha channel?
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    createInfo.presentMode = presentMode_;
    createInfo.clipped = VK_TRUE;

    // Only ever create one swap chain
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    // Create the swap chain
    if (vkCreateSwapchainKHR(device_, &createInfo, nullptr, &vkSwapChain_) != VK_SUCCESS) {
      throw std::runtime_error("failed to create swap chain!");
    }

    // Cache the handles to the swap chain images.  Note the bufferCount_ may be
    // higher than the minimum requested.
    vkGetSwapchainImagesKHR(device_, vkSwapChain_, &bufferCount_, nullptr);
    images_.resize(bufferCount_);
    vkGetSwapchainImagesKHR(device_, vkSwapChain_, &bufferCount_, images_.data());

    // Create the associated image views
    imagesView_.resize(bufferCount_);
    for(auto i = 0; i < bufferCount_; ++i) {
      imagesView_[i] = Image::createView(device, images_[i], format_.format, VK_IMAGE_ASPECT_COLOR_BIT);
    }

    cout << "Created a swap chain containing " << bufferCount_ << " images." << endl;
    return true;
}

void SwapChain::destroy() {
    // Destroy the frame buffers.
    for (auto framebuffer : frameBuffers_) {
        vkDestroyFramebuffer(device_, framebuffer, nullptr);
    }

    // Destroy the image views.
    for (auto imageView : imagesView_) {
      vkDestroyImageView(device_, imageView, nullptr);
    }

    vkDestroySwapchainKHR(device_, vkSwapChain_, nullptr);
}

VkSwapchainKHR SwapChain::getSwapChain() const {
    return vkSwapChain_;
}