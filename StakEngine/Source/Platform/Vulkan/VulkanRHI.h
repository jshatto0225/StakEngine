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
  VulkanRHI();
  ~VulkanRHI();

  Ref<IRHIDevice> createDevice(Ref<Window> window);

  void initImGui(Ref<IRHIDevice> device);
  void imGuiNewFrame();
  void shutdownImGui();
  void renderImGuiDrawData(ImDrawData *data, Ref<IRHIGraphicsContext> context);

private:
  VkResult createDebugMessenger(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT *info,
    const VkAllocationCallbacks *allocator,
    VkDebugUtilsMessengerEXT *messenger
  );
  void destroyDebugMessenger(
    VkInstance instance,
    VkDebugUtilsMessengerEXT messenger,
    const VkAllocationCallbacks *allocator
  );

private:
  VkInstance mInstance;
  VkDebugUtilsMessengerEXT mDebugMessenger;
  
private:
  VkDevice mImGuiDevice;
  VkRenderPass mImGuiRenderPass;
  VkDescriptorPool mImGuiDescriptorPool;
};

} // namespace Stak