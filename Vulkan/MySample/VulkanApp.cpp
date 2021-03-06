
#include <VulkanApp.h>

#include <Device.h>
#include <QueueSelector.h>
#include <SwapChain.h>
#include <VertexBuffer.h>
#include <CommandPool.h>
#include <SyncObjects.h>
#include <UniformBuffer.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <iostream>
#include <cstring>

using namespace std;

#define APP_SHORT_NAME "VulkanApp"
const int MAX_FRAMES_IN_FLIGHT = 2;

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
                                            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                            VkDebugUtilsMessageTypeFlagsEXT messageType,
                                            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                            void* pUserData) {
    std::string cbMessage(pCallbackData->pMessage);
    bool showMsg = true;
    // Filter out extension messages.
    if (cbMessage.find("Device Extension:") != std::string::npos) {
        showMsg = false;
    }

    if (showMsg) {
        cout << "validation layer callback message: " << cbMessage << endl;
    }
    return VK_FALSE;
}

VulkanApp::VulkanApp()
#if NDEBUG
  : validationLayers_{}
#else
  : validationLayers_{"VK_LAYER_KHRONOS_validation"}
#endif 
  , vkSurface_{}
  , currentFrame_(0)
{
}

VulkanApp::~VulkanApp() {
    cout << "Exiting the app" << endl;
}

void VulkanApp::run() {
  init();
  update();
  deInit();
  cout << "Finished run" << endl;
}

void VulkanApp::init() {
  initGlfw();
  initVulkan();
}

void VulkanApp::createSurface() {
    if (glfwCreateWindowSurface(vkInstance_, window_, nullptr, &vkSurface_) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
}

void VulkanApp::initVulkan() {
#if 0
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    cout << extensionCount << " extensions supported as follows:\n";
    vector<VkExtensionProperties> extensions(extensionCount);
    cout << "Names: ";
    for (const auto& extension : extensions) {
        cout << extension.extensionName << " ";
    }
    cout << endl;
#endif

    // initialize the VkApplicationInfo structure
    VkApplicationInfo app_info = {};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pNext = NULL;
    app_info.pApplicationName = APP_SHORT_NAME;
    app_info.applicationVersion = 1;
    app_info.pEngineName = APP_SHORT_NAME;
    app_info.engineVersion = 1;
    app_info.apiVersion = VK_API_VERSION_1_0;

    // initialize the VkInstanceCreateInfo structure
    VkInstanceCreateInfo inst_info = {};
    inst_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    inst_info.pNext = NULL;
    inst_info.flags = 0;
    inst_info.pApplicationInfo = &app_info;

    // Check for the support of validation layers.
    if (!validationLayers_.empty()) {
        if (checkValidationLayerSupport()) {
            inst_info.enabledLayerCount = static_cast<uint32_t>(validationLayers_.size());
            inst_info.ppEnabledLayerNames = validationLayers_.data();
            cout << "Validation layers are available!" << endl;
        } else {
            throw std::runtime_error("validation layers requested, but not available!");
        }
    } else {
        inst_info.enabledLayerCount = 0;
    }

    // Get the required list of extensions based on whether validation layers are enabled or not:
    auto extensions = getRequiredExtensions();
    inst_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    inst_info.ppEnabledExtensionNames = extensions.data();

    VkResult res = vkCreateInstance(&inst_info, NULL, &vkInstance_);
    if (res == VK_ERROR_INCOMPATIBLE_DRIVER || res == VK_ERROR_LAYER_NOT_PRESENT) {
        cout << "cannot find a compatible Vulkan ICD\n";
        throw runtime_error("failed to create instance!");
        exit(-1);
    } else if (res) {
        cout << "unknown error\n";
        exit(-1);
    }

    // Setup debug messenger callback.
    initDebugMessenger();

    // Create the vulkan surface which associates the vulkan instance to the window system's 
    // window handle.
    createSurface();

    // Select the physical device, and create the associated logical device and swapchain.
    device_ = make_shared<Device>(vkInstance_, vkSurface_);
    device_->create(validationLayers_);

    maxFramesInFlight_ = device_->getSwapChain()->getBufferCount();

    cout << "Vulkan instance created." << endl;
}

void VulkanApp::update() {
    while(!glfwWindowShouldClose(window_)) {
        glfwPollEvents();
        drawFrame();
    }

    vkDeviceWaitIdle(device_->getLogicalDevice());
}

void VulkanApp::deInit() {
    deInitVulkan();
    deInitGlfw();
}

void VulkanApp::initGlfw() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window_ = glfwCreateWindow(800, 600, "Vulkan window", nullptr, nullptr);
}

void VulkanApp::deInitGlfw() {
    cout << "Destroying window: " << window_ << endl;
    glfwDestroyWindow(window_);
    glfwTerminate();
    cout << "Finished deInitGlfw" << endl;
}

void VulkanApp::deInitVulkan() {

    // Destroy the device.
    device_->destroy();
    device_.reset();

    if (!validationLayers_.empty()) {
        deInitDebugMessenger();
    }

    vkDestroyInstance(vkInstance_, NULL);
    cout << "deInitVulkan " << endl;
}

bool VulkanApp::checkValidationLayerSupport() {
    uint32_t layerCount;

    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    cout << "Checking for " << layerCount << " Validation layers." << endl;

    vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    bool layerFound = false;
    char* layerName;
    for (auto layerName : validationLayers_) {
        for (const auto& layerProperties : availableLayers) {
            cout << layerProperties.layerName << ", ";
            if (strncmp(layerName, layerProperties.layerName, strlen(layerName)) == 0) {
                layerFound |= true;
                cout << "Found layer: " << layerName << endl;
            }
        }
    }
    cout << endl;

    if (!layerFound) {
        cout << "Did not find layer: " << endl;
    }
    return layerFound;
}

std::vector<const char*> VulkanApp::getRequiredExtensions() {

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (!validationLayers_.empty()) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
    return extensions;
}

void VulkanApp::initDebugMessenger() {
    if (validationLayers_.empty()) {
        return;
    }

    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | 
                             VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
    createInfo.pUserData = nullptr; // Optional

    if (createDebugUtilsMessengerEXT(vkInstance_, &createInfo, nullptr, &debugMessenger_) != VK_SUCCESS) {
        throw std::runtime_error("failed to set up debug messenger!");
    }
}

void VulkanApp::deInitDebugMessenger() {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(vkInstance_, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(vkInstance_, debugMessenger_, nullptr);
    }
}

VkResult VulkanApp::createDebugUtilsMessengerEXT(VkInstance instance, 
                                      const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
                                      const VkAllocationCallbacks* pAllocator, 
                                      VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, 
                                                                          "vkCreateDebugUtilsMessengerEXT"); 
    return func != nullptr ? func(instance, pCreateInfo, pAllocator, pDebugMessenger) : VK_ERROR_EXTENSION_NOT_PRESENT;
}

/*
??? Acquire an image from the swap chain
??? Execute the command buffer with that image as attachment in the framebuffer
??? Return the image to the swap chain for presentation
*/
void VulkanApp::drawFrame() {

    auto syncObjects = device_->getSyncObjects();
    vkWaitForFences(device_->getLogicalDevice(), 1, &syncObjects->getInflightFences()[currentFrame_], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    // Acquire an image from the swap chain
    vkAcquireNextImageKHR(device_->getLogicalDevice(), 
                          device_->getSwapChain()->getSwapChain(), UINT64_MAX, 
                          syncObjects->getImageAvailableSemaphores()[currentFrame_], 
                          VK_NULL_HANDLE, &imageIndex);

    auto uniformBuffer = device_->getUniformBuffer();

    uniformBuffer->update(imageIndex);

    // Check if a previous frame is using this image (i.e. there is its fence to wait on)
    if (syncObjects->getImagesInflight()[imageIndex] != VK_NULL_HANDLE) {
      vkWaitForFences(device_->getLogicalDevice(), 1, &syncObjects->getImagesInflight()[imageIndex], VK_TRUE, UINT64_MAX);
    }

    // Mark the image as now being in use by this frame
    syncObjects->getImagesInflight()[imageIndex] = syncObjects->getInflightFences()[currentFrame_];

    // Passed the fence, so can start drawing.  Execute the command buffer.
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {syncObjects->getImageAvailableSemaphores()[currentFrame_]};

    // Another option would have been: VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    // Add the command buffer to execute to the submitInfo buffer to execute.
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &(device_->getCommandPool()->getCommandBuffers()[imageIndex]);

    // Use a semafore to signal it is done rendering.
    VkSemaphore signalSemaphores[] = {syncObjects->getRenderFinishedSemaphores()[currentFrame_]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    // Return the fences to an unsignaled state.
    vkResetFences(device_->getLogicalDevice(), 1, &syncObjects->getInflightFences()[currentFrame_]);

    auto graphicsQueue = device_->getQueueSelector()->getGraphicsQueue();
    if (graphicsQueue == VK_NULL_HANDLE) {
        cout << "ALERT: null graphics queue." << endl;
    }
    auto syncFence = syncObjects->getInflightFences()[currentFrame_];
    if (syncFence == VK_NULL_HANDLE) {
        cout << "ALERT: null fence." << endl;
    }

    // The last parameter references an optional fence that will be signaled when the 
    // command buffers finish execution. We???re using semaphores for synchronization, 
    // so we could just pass a VK_NULL_HANDLE.
    if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, syncFence) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    // Prepare for presentation/swap.
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    // The present wait on a end of rendering semaphore.
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {device_->getSwapChain()->getSwapChain()};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &imageIndex;
    // Present will wait on the end of rendering semaphore before it can present/swap.
    vkQueuePresentKHR(device_->getQueueSelector()->getPresentQueue(), &presentInfo);

    currentFrame_ = (currentFrame_ + 1) % MAX_FRAMES_IN_FLIGHT;
}
