#pragma once

#include <vulkan/vulkan.h>

#include "RHICore.h"
#include "IRHIDevice.h"

namespace Stak {

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

class VulkanRHIGraphicsContext;
class VulkanRHIComputeContext;
class VulkanRHIUploadContext;

struct VulkanRHIRecipt : public IRHIRecipt {
  std::vector<VkFence> fences;
};

class VulkanRHI : public IRHI {
public:
  VulkanRHI() = default;
  ~VulkanRHI() = default;

  void init();
  void shutdown();

  Ref<IRHIDevice> createDevice(Ref<Window> window);

  void initImGui(Ref<IRHIDevice> device, Ref<IRHIGraphicsContext> context);
  void imGuiNewFrame();

private:
  VkResult createDebugMessenger(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT *info,
    const VkAllocationCallbacks *allocator,
    VkDebugUtilsMessengerEXT *messenger
  );

private:
  VkInstance mInstance;
  VkDebugUtilsMessengerEXT mDebugMessenger;
};

} // namespace Stak