#include <Device.h>

#include <map>

using namespace std;

Device::Device(VkInstance vkInstance) 
: vkInstance_(vkInstance)
, physicalDevice_(VK_NULL_HANDLE)
, device_(VK_NULL_HANDLE) {
}

Device::~Device() {
    cout << "Device destructor called." << endl;
    vkDestroyDevice(device_, nullptr);
}

void Device::create(const std::vector<const char*>& validationLayers) {
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
        auto familyIndex = queueSelector_.findFamily(physicalDevice_);
        if (!familyIndex.hasValue()) {
            throw std::runtime_error("Selected device does not support a graphics queue!");
        }
    } else {
        throw std::runtime_error("failed to find a suitable GPU!");
    }
}

void Device::createLogical(const std::vector<const char*>& validationLayers) {
    // Get a suitable queue family.  In this case, a graphics queue family.
    auto familyIndex = queueSelector_.findFamily(physicalDevice_);

    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = familyIndex.value();
    queueCreateInfo.queueCount = 1;

    // TODO: set appropriate priority
    float queuePriority = 1.0f;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    // Create the logical device
    // TODO: Specify the device features.
    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    // Specify our graphics queue
    createInfo.pQueueCreateInfos = &queueCreateInfo;
    createInfo.queueCreateInfoCount = 1;
    createInfo.pEnabledFeatures = &deviceFeatures;

    // Add validation layers
    createInfo.enabledExtensionCount = 0;
    if (!validationLayers.empty()) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(physicalDevice_, &createInfo, nullptr, &device_) != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device!");
    }
    cout << "Created the logica device!!!!" << endl;
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