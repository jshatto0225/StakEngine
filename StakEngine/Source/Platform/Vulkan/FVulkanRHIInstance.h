#pragma once

#include <vulkan/vulkan.h>
#include <string>

#include "RHICore.h"
#include "IRHIInstance.h"

#ifdef SK_DEBUG
const bool ENABLE_VALIDATION_LAYERS = true;
#else
const bool ENABLE_VALIDATION_LAYERS = false;
#endif

const std::vector<const char *> VALIDATION_LAYERS = {
  "VK_LAYER_KHRONOS_validation",
};

const std::vector<const char *> DEVICE_EXTENSIONS = {
  VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

class FVulkanRHIGraphicsContext;
class FVulkanRHIComputeContext;
class FVulkanRHIUploadContext;

class FVulkanRHIInstance final : public IRHIInstance {
public:
  FVulkanRHIInstance(const std::string &AppName);
  ~FVulkanRHIInstance();

public:
  VkInstance GetVkInstance() { return mInstance; }

private:
  VkResult CreateDebugMessenger(VkInstance Instance, const VkDebugUtilsMessengerCreateInfoEXT *Info, const VkAllocationCallbacks *Allocator, VkDebugUtilsMessengerEXT *Messenger);
  void DestroyDebugMessenger(VkInstance Instance, VkDebugUtilsMessengerEXT Messenger, const VkAllocationCallbacks *Allocator);

private:
  VkInstance mInstance;
  VkDebugUtilsMessengerEXT mDebugMessenger;
};
