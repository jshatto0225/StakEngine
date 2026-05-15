
#include <cstring>
#include "Asserts.h"
#include "Log.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#define VK_USE_PLATFORM_WIN32_KHR
#else
#include <dlfcn.h>
#endif

#include "VulkanLoader.h"

#ifdef VK_USE_PLATFORM_WIN32_KHR
PFN_vkCreateWin32SurfaceKHR                 vkCreateWin32SurfaceKHR;
#endif

// Expose proc-address pointers
PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr;
PFN_vkGetDeviceProcAddr   vkGetDeviceProcAddr;

// Core/global
PFN_vkCreateInstance                        vkCreateInstance;
PFN_vkDestroyInstance                       vkDestroyInstance;
PFN_vkEnumerateInstanceLayerProperties      vkEnumerateInstanceLayerProperties;

// Instance-level
PFN_vkEnumeratePhysicalDevices              vkEnumeratePhysicalDevices;
PFN_vkEnumerateDeviceExtensionProperties    vkEnumerateDeviceExtensionProperties;

PFN_vkCreateDebugUtilsMessengerEXT          vkCreateDebugUtilsMessengerEXT;
PFN_vkDestroyDebugUtilsMessengerEXT         vkDestroyDebugUtilsMessengerEXT;

PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR vkGetPhysicalDeviceSurfaceCapabilitiesKHR;
PFN_vkGetPhysicalDeviceSurfaceFormatsKHR    vkGetPhysicalDeviceSurfaceFormatsKHR;
PFN_vkGetPhysicalDeviceSurfacePresentModesKHR vkGetPhysicalDeviceSurfacePresentModesKHR;
PFN_vkGetPhysicalDeviceSurfaceSupportKHR    vkGetPhysicalDeviceSurfaceSupportKHR;

PFN_vkGetPhysicalDeviceFeatures2            vkGetPhysicalDeviceFeatures2;
PFN_vkGetPhysicalDeviceQueueFamilyProperties vkGetPhysicalDeviceQueueFamilyProperties;
PFN_vkGetPhysicalDeviceMemoryProperties     vkGetPhysicalDeviceMemoryProperties;

PFN_vkCreateDevice                          vkCreateDevice;
PFN_vkDestroyDevice                         vkDestroyDevice;

// Device-level / core
PFN_vkGetDeviceQueue                        vkGetDeviceQueue;
PFN_vkCreateCommandPool                     vkCreateCommandPool;
PFN_vkDestroyCommandPool                    vkDestroyCommandPool;
PFN_vkAllocateCommandBuffers                vkAllocateCommandBuffers;
PFN_vkFreeCommandBuffers                    vkFreeCommandBuffers;
PFN_vkBeginCommandBuffer                    vkBeginCommandBuffer;
PFN_vkEndCommandBuffer                      vkEndCommandBuffer;
PFN_vkCreateBuffer                          vkCreateBuffer;
PFN_vkDestroyBuffer                         vkDestroyBuffer;
PFN_vkGetBufferMemoryRequirements           vkGetBufferMemoryRequirements;
PFN_vkAllocateMemory                        vkAllocateMemory;
PFN_vkFreeMemory                            vkFreeMemory;
PFN_vkMapMemory                             vkMapMemory;
PFN_vkUnmapMemory                           vkUnmapMemory;
PFN_vkBindBufferMemory                      vkBindBufferMemory;
PFN_vkGetBufferDeviceAddress                vkGetBufferDeviceAddress;
PFN_vkCreateImageView                       vkCreateImageView;
PFN_vkDestroyImageView                      vkDestroyImageView;
PFN_vkCreateSemaphore                       vkCreateSemaphore;
PFN_vkDestroySemaphore                      vkDestroySemaphore;
PFN_vkCreateFence                           vkCreateFence;
PFN_vkDestroyFence                          vkDestroyFence;
PFN_vkWaitForFences                         vkWaitForFences;
PFN_vkQueueSubmit                           vkQueueSubmit;
PFN_vkQueueSubmit2                          vkQueueSubmit2;
PFN_vkDeviceWaitIdle                        vkDeviceWaitIdle;
PFN_vkCreateShaderModule                    vkCreateShaderModule;
PFN_vkDestroyShaderModule                   vkDestroyShaderModule;
PFN_vkCreateComputePipelines                vkCreateComputePipelines;
PFN_vkCreateGraphicsPipelines               vkCreateGraphicsPipelines;
PFN_vkCmdCopyBuffer                         vkCmdCopyBuffer;
PFN_vkCmdCopyBufferToImage                  vkCmdCopyBufferToImage;
PFN_vkCmdCopyImageToBuffer                  vkCmdCopyImageToBuffer;
PFN_vkCmdBindPipeline                       vkCmdBindPipeline;
PFN_vkCmdBindIndexBuffer                    vkCmdBindIndexBuffer;
PFN_vkCmdDrawIndexed                        vkCmdDrawIndexed;
PFN_vkCmdDispatch                           vkCmdDispatch;
PFN_vkCmdDispatchIndirect                   vkCmdDispatchIndirect;
PFN_vkCmdBeginRendering                     vkCmdBeginRendering;
PFN_vkCmdEndRendering                       vkCmdEndRendering;
PFN_vkCmdPipelineBarrier2                   vkCmdPipelineBarrier2;
PFN_vkCmdSetDepthWriteEnable                vkCmdSetDepthWriteEnable;
PFN_vkCmdSetDepthCompareOp                  vkCmdSetDepthCompareOp;
PFN_vkCmdSetDepthBiasEnable                 vkCmdSetDepthBiasEnable;
PFN_vkCmdSetDepthBias                       vkCmdSetDepthBias;
PFN_vkCmdSetStencilWriteMask                vkCmdSetStencilWriteMask;
PFN_vkCmdSetStencilOp                       vkCmdSetStencilOp;
PFN_vkCmdSetStencilReference                vkCmdSetStencilReference;
PFN_vkCmdSetStencilCompareMask              vkCmdSetStencilCompareMask;
PFN_vkCmdDrawIndexedIndirect                vkCmdDrawIndexedIndirect;
PFN_vkCmdDrawIndexedIndirectCount           vkCmdDrawIndexedIndirectCount;
PFN_vkDestroyPipeline                       vkDestroyPipeline;
PFN_vkWaitSemaphores                        vkWaitSemaphores;
PFN_vkGetSemaphoreCounterValue              vkGetSemaphoreCounterValue;
PFN_vkResetCommandBuffer                    vkResetCommandBuffer;

PFN_vkGetSwapchainImagesKHR                 vkGetSwapchainImagesKHR;
PFN_vkCreateSwapchainKHR                    vkCreateSwapchainKHR;
PFN_vkDestroySwapchainKHR                   vkDestroySwapchainKHR;
PFN_vkAcquireNextImageKHR                   vkAcquireNextImageKHR;
PFN_vkQueuePresentKHR                       vkQueuePresentKHR;
PFN_vkDestroySurfaceKHR                     vkDestroySurfaceKHR;

// EXT / vendor functions used in project
PFN_vkCmdPushDataEXT                        vkCmdPushDataEXT;
PFN_vkWriteResourceDescriptorsEXT           vkWriteResourceDescriptorsEXT;
PFN_vkCmdBindResourceHeapEXT                vkCmdBindResourceHeapEXT;
PFN_vkCmdSetColorBlendEnableEXT             vkCmdSetColorBlendEnableEXT;
PFN_vkCmdSetColorBlendEquationEXT           vkCmdSetColorBlendEquationEXT;
PFN_vkCmdSetColorWriteMaskEXT               vkCmdSetColorWriteMaskEXT;
PFN_vkCmdDrawMeshTasksEXT                   vkCmdDrawMeshTasksEXT;
PFN_vkCmdDrawMeshTasksIndirectEXT           vkCmdDrawMeshTasksIndirectEXT;

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

    vkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr) GetProcAddress(g_vulkan_module, "vkGetInstanceProcAddr");
    if (!vkGetInstanceProcAddr) {
        FreeLibrary(g_vulkan_module);
        g_vulkan_module = nullptr;
        return false;
    }

    vkEnumerateInstanceLayerProperties = (PFN_vkEnumerateInstanceLayerProperties) GetProcAddress(g_vulkan_module, "vkEnumerateInstanceLayerProperties");
    if (!vkEnumerateInstanceLayerProperties) {
        FreeLibrary(g_vulkan_module);
        g_vulkan_module = nullptr;
        return false;
    }

    vkCreateInstance = (PFN_vkCreateInstance) GetProcAddress(g_vulkan_module, "vkCreateInstance");
    if (!vkCreateInstance) {
        FreeLibrary(g_vulkan_module);
        g_vulkan_module = nullptr;
        return false;
    }

    vkDestroyInstance = (PFN_vkDestroyInstance) GetProcAddress(g_vulkan_module, "vkDestroyInstance");
    if (!vkDestroyInstance) {
        FreeLibrary(g_vulkan_module);
        g_vulkan_module = nullptr;
        return false;
    }

#else

    if (g_vulkan_module) return true;
    g_vulkan_module = dlopen("libvulkan.so.1", RTLD_NOW | RTLD_LOCAL);
    if (!g_vulkan_module) return false;

    vkGetInstanceProcAddr = dlsym(g_vulkan_module, "vkGetInstanceProcAddr");
    if (!vkGetInstanceProcAddr) {
        dlclose(g_vulkan_module);
        g_vulkan_module = nullptr;
        return false;
    }

    vkEnumerateInstanceLayerProperties = (PFN_vkEnumerateInstanceLayerProperties) dlsym(g_vulkan_module, "vkEnumerateInstanceLayerProperties");
    if (!vkEnumerateInstanceLayerProperties) {
        dlclose(g_vulkan_module);
        g_vulkan_module = nullptr;
        return false;
    }

    vkCreateInstance = (PFN_vkCreateInstance) dlsym(g_vulkan_module, "vkCreateInstance");
    if (!vkCreateInstance) {
        dlclose(g_vulkan_module);
        g_vulkan_module = nullptr;
        return false;
    }

#endif

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

    // Instance-level functions
    vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT) load_instance_func(instance, "vkCreateDebugUtilsMessengerEXT");
    vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT) load_instance_func(instance, "vkDestroyDebugUtilsMessengerEXT");

    vkEnumeratePhysicalDevices = (PFN_vkEnumeratePhysicalDevices) load_instance_func(instance, "vkEnumeratePhysicalDevices");
    vkEnumerateDeviceExtensionProperties = (PFN_vkEnumerateDeviceExtensionProperties) load_instance_func(instance, "vkEnumerateDeviceExtensionProperties");
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR = (PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR) load_instance_func(instance, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
    vkGetPhysicalDeviceSurfaceFormatsKHR = (PFN_vkGetPhysicalDeviceSurfaceFormatsKHR) load_instance_func(instance, "vkGetPhysicalDeviceSurfaceFormatsKHR");
    vkGetPhysicalDeviceSurfacePresentModesKHR = (PFN_vkGetPhysicalDeviceSurfacePresentModesKHR) load_instance_func(instance, "vkGetPhysicalDeviceSurfacePresentModesKHR");
    vkGetPhysicalDeviceSurfaceSupportKHR = (PFN_vkGetPhysicalDeviceSurfaceSupportKHR) load_instance_func(instance, "vkGetPhysicalDeviceSurfaceSupportKHR");
    vkGetPhysicalDeviceFeatures2 = (PFN_vkGetPhysicalDeviceFeatures2) load_instance_func(instance, "vkGetPhysicalDeviceFeatures2");
    vkGetPhysicalDeviceQueueFamilyProperties = (PFN_vkGetPhysicalDeviceQueueFamilyProperties) load_instance_func(instance, "vkGetPhysicalDeviceQueueFamilyProperties");
    vkGetPhysicalDeviceMemoryProperties = (PFN_vkGetPhysicalDeviceMemoryProperties) load_instance_func(instance, "vkGetPhysicalDeviceMemoryProperties");
    vkCreateDevice = (PFN_vkCreateDevice) load_instance_func(instance, "vkCreateDevice");
    vkDestroyDevice = (PFN_vkDestroyDevice) load_instance_func(instance, "vkDestroyDevice");
    vkGetDeviceProcAddr = (PFN_vkGetDeviceProcAddr) load_instance_func(instance, "vkGetDeviceProcAddr");
    vkDestroySurfaceKHR = (PFN_vkDestroySurfaceKHR) load_instance_func(instance, "vkDestroySurfaceKHR");

    if (!vkEnumeratePhysicalDevices || !vkEnumerateDeviceExtensionProperties || !vkGetPhysicalDeviceFeatures2 || !vkGetPhysicalDeviceQueueFamilyProperties || !vkGetPhysicalDeviceMemoryProperties || !vkCreateDevice || !vkDestroyDevice) {
        SK_LOG_ERROR("vk_loader_init: Failed to load one or more instance-level Vulkan functions");
        ok = false;
    }

    if (!vkGetPhysicalDeviceSurfaceCapabilitiesKHR || !vkGetPhysicalDeviceSurfaceFormatsKHR || !vkGetPhysicalDeviceSurfacePresentModesKHR || !vkGetPhysicalDeviceSurfaceSupportKHR || !vkDestroySurfaceKHR) {
        SK_LOG_WARN("vk_loader_init: Surface extension not available");
    }

    if (!vkCreateDebugUtilsMessengerEXT || !vkDestroyDebugUtilsMessengerEXT) {
        SK_LOG_WARN("vk_loader_init: Debug utils extension not available");
    }

#ifdef VK_USE_PLATFORM_WIN32_KHR
    vkCreateWin32SurfaceKHR = (PFN_vkCreateWin32SurfaceKHR) load_instance_func(instance, "vkCreateWin32SurfaceKHR");
    if (!vkCreateWin32SurfaceKHR) {
        SK_LOG_WARN("vk_loader_init: Win32 surface extension not available");
    }
#endif

    return ok;
}

bool vk_load_device_functions(VkDevice device) {
    if (!vkGetInstanceProcAddr) return false;
    bool ok = true;

    // Device-level functions
    vkGetDeviceQueue = (PFN_vkGetDeviceQueue) load_device_func(device, "vkGetDeviceQueue");
    vkCreateCommandPool = (PFN_vkCreateCommandPool) load_device_func(device, "vkCreateCommandPool");
    vkDestroyCommandPool = (PFN_vkDestroyCommandPool) load_device_func(device, "vkDestroyCommandPool");
    vkAllocateCommandBuffers = (PFN_vkAllocateCommandBuffers) load_device_func(device, "vkAllocateCommandBuffers");
    vkFreeCommandBuffers = (PFN_vkFreeCommandBuffers) load_device_func(device, "vkFreeCommandBuffers");
    vkBeginCommandBuffer = (PFN_vkBeginCommandBuffer) load_device_func(device, "vkBeginCommandBuffer");
    vkEndCommandBuffer = (PFN_vkEndCommandBuffer) load_device_func(device, "vkEndCommandBuffer");
    vkCreateBuffer = (PFN_vkCreateBuffer) load_device_func(device, "vkCreateBuffer");
    vkDestroyBuffer = (PFN_vkDestroyBuffer) load_device_func(device, "vkDestroyBuffer");
    vkGetBufferMemoryRequirements = (PFN_vkGetBufferMemoryRequirements) load_device_func(device, "vkGetBufferMemoryRequirements");
    vkAllocateMemory = (PFN_vkAllocateMemory) load_device_func(device, "vkAllocateMemory");
    vkFreeMemory = (PFN_vkFreeMemory) load_device_func(device, "vkFreeMemory");
    vkMapMemory = (PFN_vkMapMemory) load_device_func(device, "vkMapMemory");
    vkUnmapMemory = (PFN_vkUnmapMemory) load_device_func(device, "vkUnmapMemory");
    vkBindBufferMemory = (PFN_vkBindBufferMemory) load_device_func(device, "vkBindBufferMemory");
    vkGetBufferDeviceAddress = (PFN_vkGetBufferDeviceAddress) load_device_func(device, "vkGetBufferDeviceAddress");
    vkCreateImageView = (PFN_vkCreateImageView) load_device_func(device, "vkCreateImageView");
    vkDestroyImageView = (PFN_vkDestroyImageView) load_device_func(device, "vkDestroyImageView");
    vkCreateSemaphore = (PFN_vkCreateSemaphore) load_device_func(device, "vkCreateSemaphore");
    vkDestroySemaphore = (PFN_vkDestroySemaphore) load_device_func(device, "vkDestroySemaphore");
    vkCreateFence = (PFN_vkCreateFence) load_device_func(device, "vkCreateFence");
    vkDestroyFence = (PFN_vkDestroyFence) load_device_func(device, "vkDestroyFence");
    vkWaitForFences = (PFN_vkWaitForFences) load_device_func(device, "vkWaitForFences");
    vkQueueSubmit = (PFN_vkQueueSubmit) load_device_func(device, "vkQueueSubmit");
    vkQueueSubmit2 = (PFN_vkQueueSubmit2) load_device_func(device, "vkQueueSubmit2");
    vkDeviceWaitIdle = (PFN_vkDeviceWaitIdle) load_device_func(device, "vkDeviceWaitIdle");
    vkCreateShaderModule = (PFN_vkCreateShaderModule) load_device_func(device, "vkCreateShaderModule");
    vkDestroyShaderModule = (PFN_vkDestroyShaderModule) load_device_func(device, "vkDestroyShaderModule");
    vkCreateComputePipelines = (PFN_vkCreateComputePipelines) load_device_func(device, "vkCreateComputePipelines");
    vkCreateGraphicsPipelines = (PFN_vkCreateGraphicsPipelines) load_device_func(device, "vkCreateGraphicsPipelines");
    vkCmdCopyBuffer = (PFN_vkCmdCopyBuffer) load_device_func(device, "vkCmdCopyBuffer");
    vkCmdCopyBufferToImage = (PFN_vkCmdCopyBufferToImage) load_device_func(device, "vkCmdCopyBufferToImage");
    vkCmdCopyImageToBuffer = (PFN_vkCmdCopyImageToBuffer) load_device_func(device, "vkCmdCopyImageToBuffer");
    vkCmdBindPipeline = (PFN_vkCmdBindPipeline) load_device_func(device, "vkCmdBindPipeline");
    vkCmdBindIndexBuffer = (PFN_vkCmdBindIndexBuffer) load_device_func(device, "vkCmdBindIndexBuffer");
    vkCmdDrawIndexed = (PFN_vkCmdDrawIndexed) load_device_func(device, "vkCmdDrawIndexed");
    vkCmdDispatch = (PFN_vkCmdDispatch) load_device_func(device, "vkCmdDispatch");
    vkCmdDispatchIndirect = (PFN_vkCmdDispatchIndirect) load_device_func(device, "vkCmdDispatchIndirect");
    vkCmdBeginRendering = (PFN_vkCmdBeginRendering) load_device_func(device, "vkCmdBeginRendering");
    vkCmdEndRendering = (PFN_vkCmdEndRendering) load_device_func(device, "vkCmdEndRendering");
    vkCmdPipelineBarrier2 = (PFN_vkCmdPipelineBarrier2) load_device_func(device, "vkCmdPipelineBarrier2");
    vkCmdSetDepthWriteEnable = (PFN_vkCmdSetDepthWriteEnable) load_device_func(device, "vkCmdSetDepthWriteEnable");
    vkCmdSetDepthCompareOp = (PFN_vkCmdSetDepthCompareOp) load_device_func(device, "vkCmdSetDepthCompareOp");
    vkCmdSetDepthBiasEnable = (PFN_vkCmdSetDepthBiasEnable) load_device_func(device, "vkCmdSetDepthBiasEnable");
    vkCmdSetDepthBias = (PFN_vkCmdSetDepthBias) load_device_func(device, "vkCmdSetDepthBias");
    vkCmdSetStencilWriteMask = (PFN_vkCmdSetStencilWriteMask) load_device_func(device, "vkCmdSetStencilWriteMask");
    vkCmdSetStencilOp = (PFN_vkCmdSetStencilOp) load_device_func(device, "vkCmdSetStencilOp");
    vkCmdSetStencilReference = (PFN_vkCmdSetStencilReference) load_device_func(device, "vkCmdSetStencilReference");
    vkCmdSetStencilCompareMask = (PFN_vkCmdSetStencilCompareMask) load_device_func(device, "vkCmdSetStencilCompareMask");
    vkCmdDrawIndexedIndirect = (PFN_vkCmdDrawIndexedIndirect) load_device_func(device, "vkCmdDrawIndexedIndirect");
    vkCmdDrawIndexedIndirectCount = (PFN_vkCmdDrawIndexedIndirectCount) load_device_func(device, "vkCmdDrawIndexedIndirectCount");
    vkDestroyPipeline = (PFN_vkDestroyPipeline) load_device_func(device, "vkDestroyPipeline");
    vkWaitSemaphores = (PFN_vkWaitSemaphores) load_device_func(device, "vkWaitSemaphores");
    vkGetSwapchainImagesKHR = (PFN_vkGetSwapchainImagesKHR) load_device_func(device, "vkGetSwapchainImagesKHR");
    vkCreateSwapchainKHR = (PFN_vkCreateSwapchainKHR) load_device_func(device, "vkCreateSwapchainKHR");
    vkDestroySwapchainKHR = (PFN_vkDestroySwapchainKHR) load_device_func(device, "vkDestroySwapchainKHR");
    vkAcquireNextImageKHR = (PFN_vkAcquireNextImageKHR) load_device_func(device, "vkAcquireNextImageKHR");
    vkQueuePresentKHR = (PFN_vkQueuePresentKHR) load_device_func(device, "vkQueuePresentKHR");
    vkCmdPushDataEXT = (PFN_vkCmdPushDataEXT) load_device_func(device, "vkCmdPushDataEXT");
    vkWriteResourceDescriptorsEXT = (PFN_vkWriteResourceDescriptorsEXT) load_device_func(device, "vkWriteResourceDescriptorsEXT");
    vkCmdBindResourceHeapEXT = (PFN_vkCmdBindResourceHeapEXT) load_device_func(device, "vkCmdBindResourceHeapEXT");
    vkCmdSetColorBlendEnableEXT = (PFN_vkCmdSetColorBlendEnableEXT) load_device_func(device, "vkCmdSetColorBlendEnableEXT");
    vkCmdSetColorBlendEquationEXT = (PFN_vkCmdSetColorBlendEquationEXT) load_device_func(device, "vkCmdSetColorBlendEquationEXT");
    vkCmdSetColorWriteMaskEXT = (PFN_vkCmdSetColorWriteMaskEXT) load_device_func(device, "vkCmdSetColorWriteMaskEXT");
    vkCmdDrawMeshTasksEXT = (PFN_vkCmdDrawMeshTasksEXT) load_device_func(device, "vkCmdDrawMeshTasksEXT");
    vkCmdDrawMeshTasksIndirectEXT = (PFN_vkCmdDrawMeshTasksIndirectEXT) load_device_func(device, "vkCmdDrawMeshTasksIndirectEXT");
    vkGetSemaphoreCounterValue = (PFN_vkGetSemaphoreCounterValue) load_device_func(device, "vkGetSemaphoreCounterValue");
    vkResetCommandBuffer = (PFN_vkResetCommandBuffer) load_device_func(device, "vkResetCommandBuffer");

    if (!vkResetCommandBuffer || !vkGetSemaphoreCounterValue || !vkGetDeviceQueue || !vkCreateCommandPool || !vkDestroyCommandPool || !vkAllocateCommandBuffers || !vkFreeCommandBuffers || !vkBeginCommandBuffer || !vkEndCommandBuffer || !vkCreateBuffer || !vkDestroyBuffer || !vkGetBufferMemoryRequirements || !vkAllocateMemory || !vkFreeMemory || !vkMapMemory || !vkUnmapMemory || !vkBindBufferMemory || !vkGetBufferDeviceAddress || !vkCreateImageView || !vkDestroyImageView || !vkCreateSemaphore || !vkDestroySemaphore || !vkCreateFence || !vkDestroyFence || !vkWaitForFences || !vkQueueSubmit || !vkQueueSubmit2 || !vkDeviceWaitIdle || !vkCreateShaderModule || !vkDestroyShaderModule || !vkCreateComputePipelines || !vkCreateGraphicsPipelines || !vkCmdCopyBuffer || !vkCmdCopyBufferToImage || !vkCmdCopyImageToBuffer || !vkCmdBindPipeline || !vkCmdBindIndexBuffer || !vkCmdDrawIndexed || !vkCmdDispatch || !vkCmdDispatchIndirect || !vkCmdBeginRendering || !vkCmdEndRendering || !vkCmdPipelineBarrier2 || !vkCmdSetDepthWriteEnable || !vkCmdSetDepthCompareOp || !vkCmdSetDepthBiasEnable || !vkCmdSetDepthBias || !vkCmdSetStencilWriteMask || !vkCmdSetStencilOp || !vkCmdSetStencilReference || !vkCmdSetStencilCompareMask || !vkCmdDrawIndexedIndirect || !vkCmdDrawIndexedIndirectCount) {
        SK_LOG_ERROR("vk_load_device_functions: Failed to load one or more device-level Vulkan functions");
        ok = false;
    }

    if (!vkGetSwapchainImagesKHR || !vkCreateSwapchainKHR || !vkDestroySwapchainKHR || !vkAcquireNextImageKHR || !vkQueuePresentKHR) {
        SK_LOG_ERROR("vk_load_device_functions: Swapchain extension not available");
        SK_LOG_ERROR("  vkGetSwapchainImagesKHR: {}", (void *) vkGetSwapchainImagesKHR);
        SK_LOG_ERROR("  vkCreateSwapchainKHR: {}", (void *) vkCreateSwapchainKHR);
        SK_LOG_ERROR("  vkDestroySwapchainKHR: {}", (void *) vkDestroySwapchainKHR);
        SK_LOG_ERROR("  vkAcquireNextImageKHR: {}", (void *) vkAcquireNextImageKHR);
        SK_LOG_ERROR("  vkQueuePresentKHR: {}", (void *) vkQueuePresentKHR);

        ok = false;
    }

    if (!vkCmdPushDataEXT || !vkWriteResourceDescriptorsEXT || !vkCmdBindResourceHeapEXT || !vkCmdSetColorBlendEnableEXT || !vkCmdSetColorBlendEquationEXT || !vkCmdSetColorWriteMaskEXT || !vkCmdDrawMeshTasksEXT || !vkCmdDrawMeshTasksIndirectEXT) {
        SK_LOG_ERROR("vk_load_device_functions: Some EXT functions not available");
        SK_LOG_ERROR("  vkCmdPushDataEXT: {}", (void *) vkCmdPushDataEXT);
        SK_LOG_ERROR("  vkWriteResourceDescriptorsEXT: {}", (void *) vkWriteResourceDescriptorsEXT);
        SK_LOG_ERROR("  vkCmdBindResourceHeapEXT: {}", (void *) vkCmdBindResourceHeapEXT);
        SK_LOG_ERROR("  vkCmdSetColorBlendEnableEXT: {}", (void *) vkCmdSetColorBlendEnableEXT);
        SK_LOG_ERROR("  vkCmdSetColorBlendEquationEXT: {}", (void *) vkCmdSetColorBlendEquationEXT);
        SK_LOG_ERROR("  vkCmdSetColorWriteMaskEXT: {}", (void *) vkCmdSetColorWriteMaskEXT);
        SK_LOG_ERROR("  vkCmdDrawMeshTasksEXT: {}", (void *) vkCmdDrawMeshTasksEXT);
        SK_LOG_ERROR("  vkCmdDrawMeshTasksIndirectEXT: {}", (void *) vkCmdDrawMeshTasksIndirectEXT);
        ok = false;
    }

    return ok;
}