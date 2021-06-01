/*
 * Vulkan Samples
 *
 * Copyright (C) 2015-2016 Valve Corporation
 * Copyright (C) 2015-2016 LunarG, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
VULKAN_SAMPLE_SHORT_DESCRIPTION
create and destroy Vulkan instance
*/

/* This is part of the draw cube progression */

#include <iostream>
#include <cstdlib>
#include <util_init.hpp>

#define APP_SHORT_NAME "vulkansamples_instance"

using namespace std;

void initInstanceExtensionNames(struct sample_info &info) {
    info.instance_extension_names.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
#ifdef __ANDROID__
    info.instance_extension_names.push_back(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
#elif defined(_WIN32)
    info.instance_extension_names.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_METAL_EXT)
    info.instance_extension_names.push_back(VK_EXT_METAL_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_WAYLAND_KHR)
    info.instance_extension_names.push_back(VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME);
#else
    info.instance_extension_names.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#endif
}

void initDeviceExtensionNames(struct sample_info &info) {
    info.device_extension_names.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
}

void createInstance(struct sample_info& info) {
    init_global_layer_properties(info);

    // // Init extension names for this instance.
    cout << "Init extension names for this instance." << endl;
    initInstanceExtensionNames(info);

    cout << "Init device extension names." << endl;
    initDeviceExtensionNames(info);

    init_instance_extension_names(info);
    init_device_extension_names(info);

    VkApplicationInfo app_info = {};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pNext = NULL;
    app_info.pApplicationName = "cube app";
    app_info.applicationVersion = 1;
    app_info.pEngineName = "myown engine";
    app_info.engineVersion = 1;
    app_info.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo inst_info = {};
    inst_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    inst_info.pNext = NULL;
    inst_info.flags = 0;
    inst_info.pApplicationInfo = &app_info;
    inst_info.enabledLayerCount = info.instance_layer_names.size();
    inst_info.ppEnabledLayerNames = info.instance_layer_names.size() ? info.instance_layer_names.data() : NULL;
    inst_info.enabledExtensionCount = info.instance_extension_names.size();
    inst_info.ppEnabledExtensionNames = info.instance_extension_names.data();

    VkResult res = vkCreateInstance(&inst_info, NULL, &info.inst);
    assert(res == VK_SUCCESS);

    cout << "Created instance: " << info.inst << " return code: " << res << endl;
}

VkResult enumerateDevices(struct sample_info& info, uint32_t gpu_count) {
    uint32_t const U_ASSERT_ONLY req_count = gpu_count;

    VkResult res = vkEnumeratePhysicalDevices(info.inst, &gpu_count, NULL);
    assert(gpu_count);
    info.gpus.resize(gpu_count);

    res = vkEnumeratePhysicalDevices(info.inst, &gpu_count, info.gpus.data());
    assert(!res && gpu_count >= req_count);

    vkGetPhysicalDeviceQueueFamilyProperties(info.gpus[0], &info.queue_family_count, NULL);
    assert(info.queue_family_count >= 1);

    info.queue_props.resize(info.queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(info.gpus[0], &info.queue_family_count, info.queue_props.data());
    assert(info.queue_family_count >= 1);

    /* This is as good a place as any to do this */
    vkGetPhysicalDeviceMemoryProperties(info.gpus[0], &info.memory_properties);
    vkGetPhysicalDeviceProperties(info.gpus[0], &info.gpu_props);
    /* query device extensions for enabled layers */
    for (auto &layer_props : info.instance_layer_properties) {
        init_device_extension_properties(info, layer_props);
    }

    return res;
}

void initDevice(struct sample_info& info) {
    VkDeviceQueueCreateInfo queue_info = {};

    vkGetPhysicalDeviceQueueFamilyProperties(info.gpus[0], &info.queue_family_count, NULL);
    assert(info.queue_family_count >= 1);

    info.queue_props.resize(info.queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(info.gpus[0], &info.queue_family_count, info.queue_props.data());
    assert(info.queue_family_count >= 1);

    bool U_ASSERT_ONLY found = false;
    for (unsigned int i = 0; i < info.queue_family_count; i++) {
        if (info.queue_props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            queue_info.queueFamilyIndex = i;
            found = true;
            break;
        }
    }
    assert(found);
    assert(info.queue_family_count >= 1);

    float queue_priorities[1] = {0.0};
    queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_info.pNext = NULL;
    queue_info.queueCount = 1;
    queue_info.pQueuePriorities = queue_priorities;

    VkDeviceCreateInfo device_info = {};
    device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_info.pNext = NULL;
    device_info.queueCreateInfoCount = 1;
    device_info.pQueueCreateInfos = &queue_info;
    device_info.enabledExtensionCount = 0;
    device_info.ppEnabledExtensionNames = NULL;
    device_info.enabledLayerCount = 0;
    device_info.ppEnabledLayerNames = NULL;
    device_info.pEnabledFeatures = NULL;

    VkDevice device;
    VkResult U_ASSERT_ONLY res = vkCreateDevice(info.gpus[0], &device_info, NULL, &device);
    assert(res == VK_SUCCESS);
    cout << "Created device: " << device << endl;
}

void initWindow(struct sample_info& info, uint32_t width, uint32_t height) {

    // set the size of the window.
#ifdef __ANDROID__
    AndroidGetWindowSize(&info.width, &info.height);
#else
    info.width = width;
    info.height = height;
#endif

    init_connection(info);

    assert(info.width > 0);
    assert(info.height > 0);

    uint32_t value_mask, value_list[32];

    info.window = xcb_generate_id(info.connection);
    cout << "info.window " << info.window << endl;

    value_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    value_list[0] = info.screen->black_pixel;
    value_list[1] = XCB_EVENT_MASK_KEY_RELEASE | XCB_EVENT_MASK_EXPOSURE;

    xcb_create_window(info.connection, XCB_COPY_FROM_PARENT, info.window, info.screen->root, 0, 0, info.width, info.height, 0,
                      XCB_WINDOW_CLASS_INPUT_OUTPUT, info.screen->root_visual, value_mask, value_list);

    /* Magic code that will send notification when window is destroyed */
    xcb_intern_atom_cookie_t cookie = xcb_intern_atom(info.connection, 1, 12, "WM_PROTOCOLS");
    xcb_intern_atom_reply_t *reply = xcb_intern_atom_reply(info.connection, cookie, 0);

    xcb_intern_atom_cookie_t cookie2 = xcb_intern_atom(info.connection, 0, 16, "WM_DELETE_WINDOW");
    info.atom_wm_delete_window = xcb_intern_atom_reply(info.connection, cookie2, 0);

    xcb_change_property(info.connection, XCB_PROP_MODE_REPLACE, info.window, (*reply).atom, 4, 32, 1,
                        &(*info.atom_wm_delete_window).atom);
    free(reply);

    xcb_map_window(info.connection, info.window);

    // Force the x/y coordinates to 100,100 results are identical in consecutive
    // runs
    const uint32_t coords[] = {100, 100};
    xcb_configure_window(info.connection, info.window, XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y, coords);
    xcb_flush(info.connection);

    xcb_generic_event_t *e;
    while ((e = xcb_wait_for_event(info.connection))) {
        if ((e->response_type & ~0x80) == XCB_EXPOSE) break;
    }
}

void initCommandBuffer(struct sample_info& info) {
    VkCommandPoolCreateInfo cmd_pool_info = {};
    cmd_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cmd_pool_info.pNext = NULL;
    cmd_pool_info.queueFamilyIndex = info.graphics_queue_family_index;
    cmd_pool_info.flags = 0;

    VkResult res = vkCreateCommandPool(info.device, &cmd_pool_info, NULL, &info.cmd_pool);
    assert(res == VK_SUCCESS);

    /* Create the command buffer from the command pool */
    VkCommandBufferAllocateInfo cmd = {};
    cmd.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmd.pNext = NULL;
    cmd.commandPool = info.cmd_pool;
    cmd.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmd.commandBufferCount = 1;

    res = vkAllocateCommandBuffers(info.device, &cmd, &info.cmd);
    assert(res == VK_SUCCESS);
}

int entry_point(int argc, char *argv[]) {
    struct sample_info info = {};

    cout << "Creating a Vulkan instance." << endl;
    createInstance(info);

    cout << "Enumerating Vulkan devices." << endl;
    enumerateDevices(info, 1);

    cout << "Init the window." << endl;
    initWindow(info, 500, 500);

    cout << "Initializing the swap chain device." << endl;
    init_swapchain_extension(info);

    cout << "Initializing the Vulkan device." << endl;
    initDevice(info);

    cout << "Initializing the command buffer." << endl;
    // init_command_pool(info);
    // initCommandBuffer(info);


    // Deinit
    // cout << "Freeing command buffers." << endl;
    // VkCommandBuffer cmd_bufs[1] = {info.cmd};
    // vkFreeCommandBuffers(info.device, info.cmd_pool, 1, cmd_bufs);
    // vkDestroyCommandPool(info.device, info.cmd_pool, NULL);

    // cout << "Destroying the device instance." << endl;
    // vkDeviceWaitIdle(info.device);
    // vkDestroyDevice(info.device, NULL);

    cout << "Destroying the Vulkan instance." << endl;
    vkDestroyInstance(info.inst, NULL);

    return 0;
}
