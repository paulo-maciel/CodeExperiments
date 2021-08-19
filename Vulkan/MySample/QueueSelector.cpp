#include <QueueSelector.h>

#include <map>
#include <iostream>
#include <vector>
#include <set>

using namespace std;

QueueSelector::QueueSelector(VkSurfaceKHR vkSurface)
: graphicsQueue_{}
, presentQueue_{}
, vkSurface_(vkSurface)
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
        if (presentSupport && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indexes.graphicsFamily = i;
            indexes.presentFamily = i;
            cout << "Queue family " << i  << " supports a graphics and present queue." << endl;
            break;
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

VkDeviceQueueCreateInfo *QueueSelector::getQueuesCreateInfo(VkPhysicalDevice physicalDevice, VkDevice device) {
    // Get a suitable queue family.  In this case, a graphics queue family.
    auto familyIndexes = findFamilies(physicalDevice);

    // Get the handles to the present and graphics queues.
    // vkGetDeviceQueue(device, familyIndexes.presentFamily.value(), 0, &presentQueue_);
    // vkGetDeviceQueue(device, familyIndexes.graphicsFamily.value(), 0, &graphicsQueue_);

    std::set<uint32_t> uniqueQueueFamilies = {familyIndexes.graphicsFamily.value(), familyIndexes.presentFamily.value()};

    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority_;
        queuesCreateInfo_.push_back(queueCreateInfo);
    }

    return queuesCreateInfo_.data();
}

uint32_t QueueSelector::getQueuesCreateInfoSize() const {
  return queuesCreateInfo_.size();
}

VkQueue QueueSelector::getGraphicsQueue() const {
  return graphicsQueue_;
}

VkQueue QueueSelector::getPresentQueue() const {
  return presentQueue_;
}