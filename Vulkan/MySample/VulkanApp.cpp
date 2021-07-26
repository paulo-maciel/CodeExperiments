
#include <VulkanApp.h>

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

void VulkanApp::initVulkan() {
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    cout << extensionCount << " extensions supported as follows:\n";
    vector<VkExtensionProperties> extensions(extensionCount);
    cout << "Names: ";
    for (const auto& extension : extensions) {
        cout << extension.extensionName << " ";
    }
    cout << endl;

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

    cout << "glfwExtensions count: " << glfwExtensionCount << endl;
    for(int i = 0; i < glfwExtensionCount; ++i) {
        cout << "Extension: " << glfwExtensions[i] << endl;
    }

    VkResult res = vkCreateInstance(&inst_info, NULL, &vkInstance_);
    if (res == VK_ERROR_INCOMPATIBLE_DRIVER) {
        cout << "cannot find a compatible Vulkan ICD\n";
        throw runtime_error("failed to create instance!");
        exit(-1);
    } else if (res) {
        cout << "unknown error\n";
        exit(-1);
    }

    // Check for the support of validation layers.
    checkValidationLayerSupport();

    cout << "Vulkan instance created." << endl;
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
    cout << "Destroying window: " << window_ << endl;
    glfwDestroyWindow(window_);
    glfwTerminate();
    cout << "Finished deInitGlfw" << endl;
}

void VulkanApp::deInitVulkan() {
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
    for (layerName : validationLayers_) {
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
