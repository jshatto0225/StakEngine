#include "VulkanLoader.h"

#include <cstring>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
#include <dlfcn.h>
#endif

// Instance-level
PFN_vkCreateDebugUtilsMessengerEXT          vkCreateDebugUtilsMessengerEXT = nullptr;
PFN_vkDestroyDebugUtilsMessengerEXT         vkDestroyDebugUtilsMessengerEXT = nullptr;

// EXT / vendor
PFN_vkCmdPushDataEXT                        vkCmdPushDataEXT = nullptr;
PFN_vkWriteResourceDescriptorsEXT           vkWriteResourceDescriptorsEXT = nullptr;
PFN_vkCmdBindResourceHeapEXT                vkCmdBindResourceHeapEXT = nullptr;
PFN_vkCmdSetColorBlendEnableEXT             vkCmdSetColorBlendEnableEXT = nullptr;
PFN_vkCmdSetColorBlendEquationEXT           vkCmdSetColorBlendEquationEXT = nullptr;
PFN_vkCmdSetColorWriteMaskEXT               vkCmdSetColorWriteMaskEXT = nullptr;
PFN_vkCmdSetColorWriteEnableEXT             vkCmdSetColorWriteEnableEXT = nullptr;
PFN_vkCmdDrawMeshTasksEXT                   vkCmdDrawMeshTasksEXT = nullptr;
PFN_vkCmdDrawMeshTasksIndirectEXT           vkCmdDrawMeshTasksIndirectEXT = nullptr;

#ifdef _WIN32
static HMODULE g_vulkan_module = nullptr;
#else
static void* g_vulkan_module = nullptr;
#endif

bool vk_loader_init() {
#ifdef _WIN32
    if (g_vulkan_module) return true;
    g_vulkan_module = LoadLibraryA("vulkan-1.dll");
    if (!g_vulkan_module) return false;
    auto addr = (FARPROC) GetProcAddress(g_vulkan_module, "vkGetInstanceProcAddr");
    if (!addr) {
        FreeLibrary(g_vulkan_module);
        g_vulkan_module = nullptr;
        return false;
    }
    vkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr) addr;
#else
    if (g_vulkan_module) return true;
    g_vulkan_module = dlopen("libvulkan.so.1", RTLD_NOW | RTLD_LOCAL);
    if (!g_vulkan_module) return false;
    auto addr = dlsym(g_vulkan_module, "vkGetInstanceProcAddr");
    if (!addr) {
        dlclose(g_vulkan_module);
        g_vulkan_module = nullptr;
        return false;
    }
    vkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr) addr;
#endif

    // get vkGetDeviceProcAddr if available
    if (vkGetInstanceProcAddr) {
        vkGetDeviceProcAddr = (PFN_vkGetDeviceProcAddr) vkGetInstanceProcAddr(VK_NULL_HANDLE, "vkGetDeviceProcAddr");
    }

    // resolve a few global/core functions
    if (vkGetInstanceProcAddr) {
        vkCreateInstance = (PFN_vkCreateInstance) vkGetInstanceProcAddr(VK_NULL_HANDLE, "vkCreateInstance");
        vkDestroyInstance = (PFN_vkDestroyInstance) vkGetInstanceProcAddr(VK_NULL_HANDLE, "vkDestroyInstance");
        vkEnumerateInstanceLayerProperties = (PFN_vkEnumerateInstanceLayerProperties) vkGetInstanceProcAddr(VK_NULL_HANDLE, "vkEnumerateInstanceLayerProperties");
        vkEnumeratePhysicalDevices = (PFN_vkEnumeratePhysicalDevices) vkGetInstanceProcAddr(VK_NULL_HANDLE, "vkEnumeratePhysicalDevices");
        vkEnumerateDeviceExtensionProperties = (PFN_vkEnumerateDeviceExtensionProperties) vkGetInstanceProcAddr(VK_NULL_HANDLE, "vkEnumerateDeviceExtensionProperties");
    }

    return vkGetInstanceProcAddr != nullptr;
}

void vk_loader_shutdown() {
#ifdef _WIN32
    if (g_vulkan_module) {
        FreeLibrary(g_vulkan_module);
        g_vulkan_module = nullptr;
    }
#else
    if (g_vulkan_module) {
        dlclose(g_vulkan_module);
        g_vulkan_module = nullptr;
    }
#endif

    vkGetInstanceProcAddr = nullptr;
    vkGetDeviceProcAddr = nullptr;
}

// helpers
static PFN_vkVoidFunction load_instance_func(VkInstance instance, const char* name) {
    if (!vkGetInstanceProcAddr) return nullptr;
    return vkGetInstanceProcAddr(instance, name);
}

static PFN_vkVoidFunction load_device_func(VkDevice device, const char* name) {
    if (vkGetDeviceProcAddr && device) {
        return vkGetDeviceProcAddr(device, name);
    }
    if (vkGetInstanceProcAddr) {
        return vkGetInstanceProcAddr(VK_NULL_HANDLE, name);
    }
    return nullptr;
}

bool vk_load_instance_functions(VkInstance instance) {
    if (!vkGetInstanceProcAddr) return false;
    bool ok = true;

    vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT) load_instance_func(instance, "vkCreateDebugUtilsMessengerEXT");
    vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT) load_instance_func(instance, "vkDestroyDebugUtilsMessengerEXT");

    if (!vkCreateDebugUtilsMessengerEXT || !vkDestroyDebugUtilsMessengerEXT) {
        ok = false;
    }

    return ok;
}

bool vk_load_device_functions(VkDevice device) {
    if (!vkGetInstanceProcAddr) return false;
    bool ok = true;

    // EXT/vender functions
    vkCmdPushDataEXT = (PFN_vkCmdPushDataEXT) load_device_func(device, "vkCmdPushDataEXT");
    vkWriteResourceDescriptorsEXT = (PFN_vkWriteResourceDescriptorsEXT) load_device_func(device, "vkWriteResourceDescriptorsEXT");
    vkCmdBindResourceHeapEXT = (PFN_vkCmdBindResourceHeapEXT) load_device_func(device, "vkCmdBindResourceHeapEXT");
    vkCmdSetColorBlendEnableEXT = (PFN_vkCmdSetColorBlendEnableEXT) load_device_func(device, "vkCmdSetColorBlendEnableEXT");
    vkCmdSetColorBlendEquationEXT = (PFN_vkCmdSetColorBlendEquationEXT) load_device_func(device, "vkCmdSetColorBlendEquationEXT");
    vkCmdSetColorWriteMaskEXT = (PFN_vkCmdSetColorWriteMaskEXT) load_device_func(device, "vkCmdSetColorWriteMaskEXT");
    vkCmdSetColorWriteEnableEXT = (PFN_vkCmdSetColorWriteEnableEXT) load_device_func(device, "vkCmdSetColorWriteEnableEXT");
    vkCmdDrawMeshTasksEXT = (PFN_vkCmdDrawMeshTasksEXT) load_device_func(device, "vkCmdDrawMeshTasksEXT");
    vkCmdDrawMeshTasksIndirectEXT = (PFN_vkCmdDrawMeshTasksIndirectEXT) load_device_func(device, "vkCmdDrawMeshTasksIndirectEXT");

    if (!vkCmdPushDataEXT || !vkWriteResourceDescriptorsEXT || !vkCmdBindResourceHeapEXT || !vkCmdSetColorBlendEnableEXT || !vkCmdSetColorBlendEquationEXT || !vkCmdSetColorWriteMaskEXT || !vkCmdSetColorWriteEnableEXT || !vkCmdDrawMeshTasksEXT || !vkCmdDrawMeshTasksIndirectEXT) {
        ok = false;
    }

    return ok;
}