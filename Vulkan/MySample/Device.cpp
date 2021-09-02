#include <Device.h>

#include <map>
#include <set>

using namespace std;

Device::Device(VkInstance vkInstance, VkSurfaceKHR vkSurface) 
: vkInstance_(vkInstance)
, physicalDevice_(VK_NULL_HANDLE)
, device_(VK_NULL_HANDLE)
, vkSurface_(vkSurface)
, deviceFeatures_{}
, requiredExtensions_{VK_KHR_SWAPCHAIN_EXTENSION_NAME} {
}

Device::~Device() {
    cout << "Device destructor called." << endl;
}

void Device::create(const std::vector<const char*>& validationLayers) {
  queueSelector_ = std::make_shared<QueueSelector>(vkSurface_, getPtr());
  swapChain_ = std::make_shared<SwapChain>(vkSurface_);

  selectPhysical();
  createLogical(validationLayers);
  auto familyIndexes = queueSelector_->findFamilies(physicalDevice_);
  swapChain_->create(this, familyIndexes);

  // Now that we have a logical device and a swap chain,
  // we can create the graphics pipeline.
  graphicsPipeline_ = std::make_shared<GraphicsPipeline>(device_, swapChain_);
  graphicsPipeline_->create();

  // Create the frame buffers.
  swapChain_->createFrameBuffers(graphicsPipeline_->getRenderPass());

  // Create the command pool and the command buffers.
  commandPool_ = std::make_shared<CommandPool>(getPtr());
  commandPool_->create();

  // Create the vertex buffer
  const std::vector<VertexBuffer::Vertex> vertices = {
      {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
      {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
      {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
      {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}};

  const std::vector<uint16_t> indices = {
      0, 1, 2, 2, 3, 0};

  vertexBuffer_ = std::make_unique<VertexBuffer>(getPtr(), vertices, indices);
  vertexBuffer_->create(commandPool_->getCommandPool(), queueSelector_->getGraphicsQueue());

  // Create the command buffers.
  commandPool_->createCommandBuffers(vertexBuffer_);

  syncObjects_ = std::make_unique<SyncObjects>(getPtr());
  syncObjects_->create();

  cout << "Device created." << endl;
}

void Device::selectPhysical() {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(vkInstance_, &deviceCount, nullptr);
    if (deviceCount == 0) {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }
    cout << "Got " << deviceCount << " devices." << endl;
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(vkInstance_, &deviceCount, devices.data());

    // Use an ordered map to automatically sort candidates by increasing score
    multimap<int, VkPhysicalDevice> candidates;
    for (const auto& device : devices) {
        int score = rateDevice(device);
        candidates.insert(std::make_pair(score, device));
    }

    // Check if the best candidate is suitable at all.
    cout << "Got " << candidates.size() << " candidate devices to choose from." << endl;
    if (candidates.rbegin()->first > 0) {
        physicalDevice_ = candidates.rbegin()->second;

        // Check if the device supports a GraphicsQueue
        if (!queueSelector_->hasValidFamilies(physicalDevice_)) {
            throw std::runtime_error("Selected device does not support a graphics queue!");
        }
    } else {
        throw std::runtime_error("failed to find a suitable GPU!");
    }

    cout << "Selected physical device!" << endl;
}

void Device::createLogical(const std::vector<const char*>& validationLayers) {
    cout << "Creating logical device...." << endl;

    // Specify our graphics queue
    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = queueSelector_->getQueuesCreateInfo();
    createInfo.queueCreateInfoCount = queueSelector_->getQueuesCreateInfoSize();
    createInfo.pEnabledFeatures = &deviceFeatures_;

    // Add required extension names (already validated in rateDevice)
    createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions_.size());
    createInfo.ppEnabledExtensionNames = requiredExtensions_.data();

    // Add validation layers
    if (!validationLayers.empty()) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }
    cout << "Calling vkCreateDevice on " << physicalDevice_ << endl;
    if (vkCreateDevice(physicalDevice_, &createInfo, nullptr, &device_) != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device!");
    }

    cout << "Created the logical device." << endl;

    // Cache the graphics and present queues
    queueSelector_->cacheQueues(device_);
}

int Device::rateDevice(const VkPhysicalDevice& physicalDevice) {
    int score = 0;

    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;

    vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
    vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);

    // Discrete GPUs have a significant performance advantage.
    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        score += 10000;
    }

    // Maximum possible size of textures affects graphics quality
    score += deviceProperties.limits.maxImageDimension2D;

    // Application can't function without geometry shaders
    if (!deviceFeatures.geometryShader) {
        cout << "Device does not support geometry shader." << endl;
        score = -1;
    } else {
        score += 10000;
    }

    if (!checkForRequiredExtension(physicalDevice)) {
        cout << "Device does not support all of the required extensions." << endl;
        score = -1;
    } else {
        score += 10000;
    }

    // Query the swap chain.  Require that at least one format and one present mode is present.
    SwapChain::Details swapChainDetails = swapChain_->getDetails(physicalDevice);
    if (swapChainDetails.formats.empty() || swapChainDetails.presentModes.empty()) {
        cout << "Device does not support at least one swapchain format or present mode." << endl;
        score = -1;
    } else {
        score += 10000;
    }

    cout << "Total device score: " << score << endl;
    return score;
}

bool Device::checkForRequiredExtension(VkPhysicalDevice device) const {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(requiredExtensions_.begin(), requiredExtensions_.end());

    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }
    return requiredExtensions.empty();
}

VkPhysicalDevice Device::getPhysicalDevice() const {
  return physicalDevice_;
}

VkDevice Device::getLogicalDevice() const {
  return device_;
}

std::shared_ptr<QueueSelector> Device::getQueueSelector() const {
  return queueSelector_;
}

std::shared_ptr<SwapChain> Device::getSwapChain() const {
  return swapChain_;
}

std::shared_ptr<GraphicsPipeline> Device::getGraphicsPipeline() const {
    return graphicsPipeline_;
}

std::shared_ptr<CommandPool> Device::getCommandPool() const {
    return commandPool_;
}

std::shared_ptr<SyncObjects> Device::getSyncObjects() const {
    return syncObjects_;
}

void Device::destroy() {
    cout << "Destroying the sync objects. " << endl;
    syncObjects_->destroy();

    cout << "Destroying the command pool. " << endl;
    commandPool_->destroy();

    cout << "Destroying the vertex buffer. " << endl;
    vertexBuffer_->destroy();

    cout << "Destroying Vulkan pipeline. " << endl;
    graphicsPipeline_->destroy();

    cout << "Destroying the Vulkan swap chain. " << endl;
    swapChain_->destroy();

    cout << "Destroying Vulkan surface: " << vkSurface_ << endl;
    vkDestroySurfaceKHR(vkInstance_, vkSurface_, nullptr);

    vkDestroyDevice(device_, nullptr);
}