#pragma once

#include "IRHIDevice.h"
#include "FVulkanRHIInstance.h"
#include "VulkanRHICore.h"

#include <vulkan/vulkan.h>

#include <optional>
#include <set>

class FVulkanRHIDevice final : public IRHIDevice {
public:
  FVulkanRHIDevice(TRef<FVulkanRHIInstance> Instance, TRef<IWindow> Window);
  ~FVulkanRHIDevice();

  ERHIFormat GetSwapchainImageFormat() { return VulkanRHIGetERHIFormat(mSwapchainImageFormat.format); };
  FUInt32 GetSwapchainImageCount() { return mSwapchainImages.size(); }

  TRef<IRHIWorkRecipt> SubmitWork(TRef<IRHIContext> Context);
  void WaitOnWork(TRef<IRHIWorkRecipt> Recipt);
  void Present();

  FUInt32 GetCurrentFrameIndex() { return mCurrentFrame; }

  FUInt32 GetSwapchainWidth() const { return mExtent.width; }
  FUInt32 GetSwapchainHeight() const { return mExtent.height; }
  FUInt32 GetSwapchainLayers() const { return 1; }

public:
  inline std::vector<VkImageView> GetVkSwapchainImageViews() const { return mSwapchainImageViews; }
  inline std::vector<VkImage> GetVkSwapchainImages() const { return mSwapchainImages; }
  inline VkDevice GetVkDevice() const { return mDevice; }

private:
  struct VulkanSwapchainSupport {
    VkSurfaceCapabilitiesKHR Capabilities;
    std::vector<VkSurfaceFormatKHR> Formats;
    std::vector<VkPresentModeKHR> PresentModes;
  };
  struct VulkanQueueFamilies {
    std::optional<FUInt32> GraphicsFamily;
    std::optional<FUInt32> PresentFamily;
  };

private:
  VulkanQueueFamilies FindQueueFamilies(VkPhysicalDevice Device);
  VulkanSwapchainSupport GetSwapchainSupport(VkPhysicalDevice Device);
  void CreateSwapchain();
  void CreateImageViews();
  VkImageView CreateImageView(VkImage Image, VkFormat Format);
  void RecreateSwapchain();

private:
  TRef<IWindow> mWindow;

private:
  TRef<FVulkanRHIInstance> mInstance;

private:
  VkSurfaceKHR mSurface;
  VkPhysicalDevice mPhysicalDevice;
  VkDevice mDevice;
  VkQueue mGraphicsQueue;
  FUInt32 mGraphicsQueueFamily;
  VkQueue mPresentQueue;
  VkExtent2D mExtent;
  VkSwapchainKHR mSwapchain;
  std::vector<VkImage> mSwapchainImages;
  VkSurfaceFormatKHR mSwapchainImageFormat;
  std::vector<VkImageView> mSwapchainImageViews;
  VkCommandPool mCommandPool;
  VkDescriptorPool mImGuiPool;
};
