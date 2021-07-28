#include <QueueSelector.h>

#include <map>
#include <iostream>
#include <vector>

using namespace std;

QueueSelector::QueueSelector()  {
}

QueueSelector::~QueueSelector() {
    cout << "QueueSelector destructor called." << endl;
}

QueueSelector::QueueFamilyIndex QueueSelector::findFamily(const VkPhysicalDevice& device) {
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
            cout << "Devices supports a graphics queue." << endl;
        }
        i++;
    }

    return index;
}

bool QueueSelector::QueueFamilyIndex::hasValue() {
  return graphicsFamily != experimental::fundamentals_v1::nullopt;
}

uint32_t QueueSelector::QueueFamilyIndex::value() {
    return graphicsFamily.value();
}