#include <DeviceSelector.h>

#include <map>

using namespace std;

DeviceSelector::DeviceSelector() 
: physicalDevice_(VK_NULL_HANDLE) {
}

DeviceSelector::~DeviceSelector() {
    cout << "DeviceSelector destructor called." << endl;
}

void DeviceSelector::select(const VkInstance& vkInstance) {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(vkInstance, &deviceCount, nullptr);
    if (deviceCount == 0) {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }
    cout << "Got " << deviceCount << " devices." << endl;
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(vkInstance, &deviceCount, devices.data());

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

int DeviceSelector::rateDevice(const VkPhysicalDevice& physicalDevice) const {
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