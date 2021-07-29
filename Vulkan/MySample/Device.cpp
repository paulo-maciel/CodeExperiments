#include <Device.h>

#include <map>

using namespace std;

Device::Device(VkInstance vkInstance, VkSurfaceKHR vkSurface) 
: vkInstance_(vkInstance)
, physicalDevice_(VK_NULL_HANDLE)
, device_(VK_NULL_HANDLE)
, vkSurface_(vkSurface)
, deviceFeatures_{} {
}

Device::~Device() {
    cout << "Device destructor called." << endl;
    vkDestroyDevice(device_, nullptr);
}

void Device::create(const std::vector<const char*>& validationLayers) {
  queueSelector_ = std::make_unique<QueueSelector>(vkSurface_);
  selectPhysical();
  createLogical(validationLayers);
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
    createInfo.pQueueCreateInfos = queueSelector_->getQueuesCreateInfo(physicalDevice_, device_);
    createInfo.queueCreateInfoCount = queueSelector_->getQueuesCreateInfoSize();
    createInfo.pEnabledFeatures = &deviceFeatures_;

    // Add validation layers
    createInfo.enabledExtensionCount = 0;
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
}

int Device::rateDevice(const VkPhysicalDevice& physicalDevice) const {
    int score = 0;

    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;

    vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
    vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);

    // Discrete GPUs have a significant performance advantage.
    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        score += 1000;
    }

    // Maximum possible size of textures affects graphics quality
    score += deviceProperties.limits.maxImageDimension2D;

    // Application can't function without geometry shaders
    if (!deviceFeatures.geometryShader) {
        return 0;
    }

    return score;
}