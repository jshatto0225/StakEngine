#pragma once

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>
#include <cstdint>

#ifdef __cplusplus
extern "C" {
#endif

// Loader init / shutdown
bool vk_loader_init();
void vk_loader_shutdown();

// Loaders
bool vk_load_instance_functions(VkInstance instance);
bool vk_load_device_functions(VkDevice device);

// Expose proc-address pointers
extern PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr;
extern PFN_vkGetDeviceProcAddr   vkGetDeviceProcAddr;

// Core/global
extern PFN_vkCreateInstance                        vkCreateInstance;
extern PFN_vkDestroyInstance                       vkDestroyInstance;
extern PFN_vkEnumerateInstanceLayerProperties      vkEnumerateInstanceLayerProperties;
extern PFN_vkEnumeratePhysicalDevices              vkEnumeratePhysicalDevices;
extern PFN_vkEnumerateDeviceExtensionProperties    vkEnumerateDeviceExtensionProperties;

// Instance-level
extern PFN_vkCreateDebugUtilsMessengerEXT          vkCreateDebugUtilsMessengerEXT;
extern PFN_vkDestroyDebugUtilsMessengerEXT         vkDestroyDebugUtilsMessengerEXT;

extern PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR vkGetPhysicalDeviceSurfaceCapabilitiesKHR;
extern PFN_vkGetPhysicalDeviceSurfaceFormatsKHR    vkGetPhysicalDeviceSurfaceFormatsKHR;
extern PFN_vkGetPhysicalDeviceSurfacePresentModesKHR vkGetPhysicalDeviceSurfacePresentModesKHR;
extern PFN_vkGetPhysicalDeviceSurfaceSupportKHR    vkGetPhysicalDeviceSurfaceSupportKHR;

extern PFN_vkGetPhysicalDeviceFeatures2            vkGetPhysicalDeviceFeatures2;
extern PFN_vkGetPhysicalDeviceQueueFamilyProperties vkGetPhysicalDeviceQueueFamilyProperties;
extern PFN_vkGetPhysicalDeviceMemoryProperties     vkGetPhysicalDeviceMemoryProperties;

// Device-level / core
extern PFN_vkCreateDevice                          vkCreateDevice;
extern PFN_vkDestroyDevice                         vkDestroyDevice;
extern PFN_vkGetDeviceQueue                        vkGetDeviceQueue;
extern PFN_vkCreateCommandPool                     vkCreateCommandPool;
extern PFN_vkDestroyCommandPool                    vkDestroyCommandPool;
extern PFN_vkAllocateCommandBuffers                vkAllocateCommandBuffers;
extern PFN_vkFreeCommandBuffers                    vkFreeCommandBuffers;
extern PFN_vkBeginCommandBuffer                    vkBeginCommandBuffer;
extern PFN_vkEndCommandBuffer                      vkEndCommandBuffer;
extern PFN_vkCreateBuffer                          vkCreateBuffer;
extern PFN_vkDestroyBuffer                         vkDestroyBuffer;
extern PFN_vkGetBufferMemoryRequirements           vkGetBufferMemoryRequirements;
extern PFN_vkAllocateMemory                        vkAllocateMemory;
extern PFN_vkFreeMemory                            vkFreeMemory;
extern PFN_vkMapMemory                             vkMapMemory;
extern PFN_vkUnmapMemory                           vkUnmapMemory;
extern PFN_vkBindBufferMemory                      vkBindBufferMemory;
extern PFN_vkGetBufferDeviceAddress                vkGetBufferDeviceAddress;
extern PFN_vkCreateImageView                       vkCreateImageView;
extern PFN_vkDestroyImageView                      vkDestroyImageView;
extern PFN_vkCreateSemaphore                       vkCreateSemaphore;
extern PFN_vkDestroySemaphore                      vkDestroySemaphore;
extern PFN_vkCreateFence                           vkCreateFence;
extern PFN_vkDestroyFence                          vkDestroyFence;
extern PFN_vkWaitForFences                         vkWaitForFences;
extern PFN_vkQueueSubmit                           vkQueueSubmit;
extern PFN_vkQueueSubmit2                          vkQueueSubmit2;
extern PFN_vkDeviceWaitIdle                        vkDeviceWaitIdle;
extern PFN_vkCreateShaderModule                    vkCreateShaderModule;
extern PFN_vkDestroyShaderModule                   vkDestroyShaderModule;
extern PFN_vkCreateComputePipelines                vkCreateComputePipelines;
extern PFN_vkCreateGraphicsPipelines               vkCreateGraphicsPipelines;
extern PFN_vkCmdCopyBuffer                         vkCmdCopyBuffer;
extern PFN_vkCmdCopyBufferToImage                  vkCmdCopyBufferToImage;
extern PFN_vkCmdCopyImageToBuffer                  vkCmdCopyImageToBuffer;
extern PFN_vkCmdBindPipeline                       vkCmdBindPipeline;
extern PFN_vkCmdBindIndexBuffer                    vkCmdBindIndexBuffer;
extern PFN_vkCmdDrawIndexed                        vkCmdDrawIndexed;
extern PFN_vkCmdDispatch                           vkCmdDispatch;
extern PFN_vkCmdDispatchIndirect                   vkCmdDispatchIndirect;
extern PFN_vkCmdBeginRendering                     vkCmdBeginRendering;
extern PFN_vkCmdEndRendering                       vkCmdEndRendering;
extern PFN_vkCmdPipelineBarrier2                   vkCmdPipelineBarrier2;
extern PFN_vkCmdSetDepthWriteEnable                vkCmdSetDepthWriteEnable;
extern PFN_vkCmdSetDepthCompareOp                  vkCmdSetDepthCompareOp;
extern PFN_vkCmdSetDepthBiasEnable                 vkCmdSetDepthBiasEnable;
extern PFN_vkCmdSetDepthBias                       vkCmdSetDepthBias;
extern PFN_vkCmdSetStencilWriteMask                vkCmdSetStencilWriteMask;
extern PFN_vkCmdSetStencilOp                       vkCmdSetStencilOp;
extern PFN_vkCmdSetStencilReference                vkCmdSetStencilReference;
extern PFN_vkCmdSetStencilCompareMask              vkCmdSetStencilCompareMask;
extern PFN_vkCmdDrawIndexedIndirect                vkCmdDrawIndexedIndirect;
extern PFN_vkCmdDrawIndexedIndirectCount           vkCmdDrawIndexedIndirectCount;
extern PFN_vkDestroyPipeline                       vkDestroyPipeline;
extern PFN_vkWaitSemaphores                        vkWaitSemaphores;

extern PFN_vkGetSwapchainImagesKHR                 vkGetSwapchainImagesKHR;
extern PFN_vkCreateSwapchainKHR                    vkCreateSwapchainKHR;
extern PFN_vkDestroySwapchainKHR                   vkDestroySwapchainKHR;
extern PFN_vkAcquireNextImageKHR                   vkAcquireNextImageKHR;
extern PFN_vkQueuePresentKHR                       vkQueuePresentKHR;

// EXT / vendor functions used in project
extern PFN_vkCmdPushDataEXT                        vkCmdPushDataEXT;
extern PFN_vkWriteResourceDescriptorsEXT           vkWriteResourceDescriptorsEXT;
extern PFN_vkCmdBindResourceHeapEXT                vkCmdBindResourceHeapEXT;
extern PFN_vkCmdSetColorBlendEnableEXT             vkCmdSetColorBlendEnableEXT;
extern PFN_vkCmdSetColorBlendEquationEXT           vkCmdSetColorBlendEquationEXT;
extern PFN_vkCmdSetColorWriteMaskEXT               vkCmdSetColorWriteMaskEXT;
extern PFN_vkCmdSetColorWriteEnableEXT             vkCmdSetColorWriteEnableEXT;
extern PFN_vkCmdDrawMeshTasksEXT                   vkCmdDrawMeshTasksEXT;
extern PFN_vkCmdDrawMeshTasksIndirectEXT           vkCmdDrawMeshTasksIndirectEXT;

#ifdef __cplusplus
}
#endif