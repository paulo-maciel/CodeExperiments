#include <QueueSelector.h>

#include <map>
#include <iostream>
#include <vector>

using namespace std;

QueueSelector::QueueSelector()
: queueCreateInfo_{}
, queuePriority_(1.0f) { // TODO: Set the appropriate priority.
}

QueueSelector::~QueueSelector() {
    cout << "QueueSelector destructor called." << endl;
}

QueueSelector::QueueFamilyIndex QueueSelector::findFamily(const VkPhysicalDevice& device) const {
    QueueSelector::QueueFamilyIndex index;

    // Logic to find queue family index to populate struct with
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
        cout << "Testing family: " << i << endl;
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            index.graphicsFamily = i;
            cout << "Queue family " << i  << " supports a graphics queue." << endl;
        }
        i++;
    }

    return index;
}

bool QueueSelector::hasValidFamily(VkPhysicalDevice physicalDevice) {
    auto familyIndex = findFamily(physicalDevice);
    return familyIndex.hasValue();
}

bool QueueSelector::QueueFamilyIndex::hasValue() {
  return graphicsFamily != experimental::fundamentals_v1::nullopt;
}

uint32_t QueueSelector::QueueFamilyIndex::value() {
    return graphicsFamily.value();
}

const VkDeviceQueueCreateInfo& QueueSelector::getQueueCreateInfo(VkPhysicalDevice physicalDevice) {
    // Get a suitable queue family.  In this case, a graphics queue family.
    auto familyIndex = findFamily(physicalDevice);

    if (familyIndex.hasValue()) {
        cout << "Family index has value: " << familyIndex.value() << endl;
        queueCreateInfo_.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo_.queueFamilyIndex = familyIndex.value();
        queueCreateInfo_.queueCount = 1;
        queueCreateInfo_.pQueuePriorities = &queuePriority_;
    }

    return queueCreateInfo_;
}