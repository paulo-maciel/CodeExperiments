
#include <VulkanApp.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <iostream>
#include <cstring>


#define APP_SHORT_NAME "VulkanApp"

VulkanApp::VulkanApp()
#if NDEBUG
  : enableValidationLayers(false)
#else
  : enableValidationLayers(true)
  , validationLayers_{"VK_LAYER_KHRONOS_validation"}
#endif 
{
}

VulkanApp::~VulkanApp() {
    std::cout << "Exiting the app" << std::endl;
}

void VulkanApp::run() {
  init();
  update();
  deInit();
  std::cout << "Finished run" << std::endl;
}

void VulkanApp::init() {
  initGlfw();
  initVulkan();
}

void VulkanApp::initVulkan() {
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::cout << extensionCount << " extensions supported as follows:\n";
    std::vector<VkExtensionProperties> extensions(extensionCount);
    for (const auto& extension : extensions) {
        std::cout <<  "Name: " << extension.extensionName << '\n';
    }

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
    inst_info.enabledExtensionCount = 0;
    inst_info.ppEnabledExtensionNames = NULL;
    inst_info.enabledLayerCount = 0;
    inst_info.ppEnabledLayerNames = NULL;

    // Enable the vulkan extensions via glfw
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions =  glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    inst_info.enabledExtensionCount = glfwExtensionCount;
    inst_info.ppEnabledExtensionNames = glfwExtensions;

    std::cout << "glfwExtensions count: " << glfwExtensionCount << std::endl;
    for(int i = 0; i < glfwExtensionCount; ++i) {
        std::cout << "Extension: " << glfwExtensions[i] << std::endl;
    }

    VkResult res = vkCreateInstance(&inst_info, NULL, &vkInstance_);
    if (res == VK_ERROR_INCOMPATIBLE_DRIVER) {
        std::cout << "cannot find a compatible Vulkan ICD\n";
        throw std::runtime_error("failed to create instance!");
        exit(-1);
    } else if (res) {
        std::cout << "unknown error\n";
        exit(-1);
    }

    std::cout << "Vulkan instance created." << std::endl;
}

void VulkanApp::update() {
    while(!glfwWindowShouldClose(window_)) {
        glfwPollEvents();
    }
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
    std::cout << "Destroying window: " << window_ << std::endl;
    glfwDestroyWindow(window_);
    glfwTerminate();
    std::cout << "Finished deInitGlfw" << std::endl;
}

void VulkanApp::deInitVulkan() {
    vkDestroyInstance(vkInstance_, NULL);
    std::cout << "deInitVulkan " << std::endl;
}

bool VulkanApp::checkValidationLayerSupport() {
    uint32_t layerCount;

    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    bool layerFound = false;
    char* layerName;
    for (layerName : validationLayers_) {
        for (const auto& layerProperties : availableLayers) {
            if (strncmp(layerName, layerProperties.layerName, strlen(layerName)) == 0) {
                layerFound |= true;
                std::cout << "Found layer: " << layerName << std::endl;
            }
        }
    }

    if (!layerFound) {
        std::cout << "Did not find layer: " << std::endl;
    }
    return layerFound;
}
