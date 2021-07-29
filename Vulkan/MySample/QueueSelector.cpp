#include <QueueSelector.h>

#include <map>
#include <iostream>
#include <vector>

using namespace std;

QueueSelector::QueueSelector(VkSurfaceKHR vkSurface)
: vkQueue_{}
, vkSurface_(vkSurface)
, queueCreateInfo_{}
, queuePriority_(1.0f) { // TODO: Set the appropriate priority.
}

QueueSelector::~QueueSelector() {
    cout << "QueueSelector destructor called." << endl;
}

QueueSelector::QueueFamilyIndices QueueSelector::findFamilies(const VkPhysicalDevice& device) const {
    QueueSelector::QueueFamilyIndices indexes;

    // Logic to find queue family indexes to populate struct with
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    VkBool32 presentSupport = false;
    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
        cout << "Testing family: " << i << endl;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, vkSurface_, &presentSupport);
        if (presentSupport) {
          indexes.presentFamily = i;
        }
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indexes.graphicsFamily = i;
            cout << "Queue family " << i  << " supports a graphics queue." << endl;
        }
        i++;
    }

    return indexes;
}

bool QueueSelector::hasValidFamilies(VkPhysicalDevice physicalDevice) {
    auto familyIndexes = findFamilies(physicalDevice);
    return familyIndexes.hasValue();
}

bool QueueSelector::QueueFamilyIndices::hasValue() {
  return (graphicsFamily != experimental::fundamentals_v1::nullopt &&
          presentFamily != experimental::fundamentals_v1::nullopt  &&
          graphicsFamily.value() == presentFamily.value());
}

uint32_t QueueSelector::QueueFamilyIndices::value() {
    return graphicsFamily.value();
}

const VkDeviceQueueCreateInfo& QueueSelector::getQueueCreateInfo(VkPhysicalDevice physicalDevice) {
    // Get a suitable queue family.  In this case, a graphics queue family.
    auto familyIndexes = findFamilies(physicalDevice);

    if (familyIndexes.hasValue()) {
        cout << "Family indexes has value: " << familyIndexes.value() << endl;
        queueCreateInfo_.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo_.queueFamilyIndex = familyIndexes.value();
        queueCreateInfo_.queueCount = 1;
        queueCreateInfo_.pQueuePriorities = &queuePriority_;
    }

    return queueCreateInfo_;
}