#include <SwapChain.h>

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


SwapChain::Details SwapChain::getDetails(VkPhysicalDevice device) {
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