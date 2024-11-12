#pragma once

#include "IRHIDevice.h"

#include <vulkan/vulkan.h>

#include <optional>
#include <set>

namespace Stak {

class VulkanRHIDevice : public IRHIDevice {
public:
  VulkanRHIDevice(VkInstance instance, Ref<Window> window);
  ~VulkanRHIDevice();

  void processWindowChanges(Ref<Window> window);
  Ref<IRHIBuffer> createBuffer(RHIBufferDescription &bufferDesc);
  Ref<IRHITexture> createTexture(RHITextureDescription &textureDesc);
  Ref<IRHIShader> createShader(RHIShaderDescription &shaderDesc);
  Ref<IRHIPipeline> createPipeline(RHIPipelineDescription &pipelineDesc);
  Ref<IRHIGraphicsContext> createGraphicsContext();

  Scope<IRHIRecipt> submitWork(Ref<IRHIContext> context);
  void waitOnWork(Scope<IRHIRecipt> recipt);

  void present();

public:
  VkFormat getSwapchainImageFormat() { return mSwapchainImageFormat.format; }

private:
  struct VulkanSwapchainSupport {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
  };
  struct VulkanQueueFamilies {
    std::optional<u32> graphicsFamily;
    std::optional<u32> presentFamily;
  };

private:
  VulkanQueueFamilies findQueueFamilies(VkPhysicalDevice device);
  VulkanSwapchainSupport getSwapchainSupport(VkPhysicalDevice device);

private:
  void createSwapchain(Ref<Window> window);
  void createImageViews();
  VkImageView createImageView(VkImage image, VkFormat format);
  void recreateSwapchain(Ref<Window> window);

public:
  inline VkPhysicalDevice getPhysicalDevice() { return mPhysicalDevice; }
  inline VkDevice getDevice() { return mDevice; }
  inline u32 getGraphicsQueueFamily() { return mGraphicsQueueFamily; }
  inline VkQueue getGraphicsQueue() { return mGraphicsQueue; }
  inline u32 getImageCount() { return mSwapchainImages.size(); }

private:
  VkInstance mInstance;
  VkSurfaceKHR mSurface;
  VkPhysicalDevice mPhysicalDevice;
  VkDevice mDevice;
  VkQueue mGraphicsQueue;
  u32 mGraphicsQueueFamily;
  VkQueue mPresentQueue;
  VkExtent2D mExtent;
  VkSwapchainKHR mSwapchain;
  std::vector<VkImage> mSwapchainImages;
  VkSurfaceFormatKHR mSwapchainImageFormat;
  std::vector<VkImageView> mSwapchainImageViews;
};

} // namespace Stak