#pragma once

#include "IRHIDevice.h"
#include "FVulkanRHIInstance.h"
#include "VulkanRHICore.h"

#include <vulkan/vulkan.h>

#include <optional>
#include <set>

class FVulkanRHIDevice final : public IRHIDevice {
public:
  FVulkanRHIDevice(TRef<FVulkanRHIInstance> Instance, TRef<IWindow> Window, FUInt32 MaxFramesInFlight);
  ~FVulkanRHIDevice();

  ERHIFormat GetSwapchainImageFormat() { return VulkanRHIGetERHIFormat(mSwapchainImageFormat.format); };
  FUInt32 GetSwapchainImageCount() { return static_cast<FUInt32>(mSwapchainImages.size()); }

  TRef<IRHIWorkRecipt> SubmitWork(TRef<IRHIContext> Context) override;
  void WaitOnWork(TRef<IRHIWorkRecipt> Recipt) override;
  void Present() override;

  FUInt32 GetCurrentFrameIndex() const override { return mCurrentFrame; }

  FUInt32 GetSwapchainWidth() const override { return mExtent.width; }
  FUInt32 GetSwapchainHeight() const override { return mExtent.height; }
  FUInt32 GetSwapchainLayers() const override { return 1; }

  inline FUInt32 GetCurrentSwapchainImageIndex() const override { return mSwapchainImageIndex; }
  inline FUInt32 GetMaxFramesInFlight() const override { return mMaxFramesInFlight; }

public:
  inline VkImageView GetCurrentVkSwapchainImageView() const { return mSwapchainImageViews[mSwapchainImageIndex]; }
  inline VkImage GetCurrentVkSwapchainImage() const { return mSwapchainImages[mSwapchainImageIndex]; }
  inline VkDevice GetVkDevice() const { return mDevice; }
  inline VkFormat GetVkSwapchainImageFormat() const { return mSwapchainImageFormat.format; }

  inline FUInt32 GetVkGraphicsQueueFamilyIndex() const { return mGraphicsQueueFamily; }
  inline VkQueue GetVkGraphicsQueue() const { return mGraphicsQueue; }
  inline VkPhysicalDevice GetVkPhysicalDevice() const { return mPhysicalDevice; }
  inline VkDescriptorPool GetImGuiVkDescriptorPool() const { return mImGuiPool; }
  void FreeImGuiVkDescriptorPool();

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
  void CreateSyncObjects();

private:
  TRef<IWindow> mWindow;

private:
  TRef<FVulkanRHIInstance> mInstance;

private:
  VkSurfaceKHR mSurface = VK_NULL_HANDLE;
  VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
  VkDevice mDevice = VK_NULL_HANDLE;
  VkQueue mGraphicsQueue = VK_NULL_HANDLE;
  FUInt32 mGraphicsQueueFamily = UINT32_MAX;
  FUInt32 mSwapchainImageIndex = UINT32_MAX;
  VkQueue mPresentQueue = VK_NULL_HANDLE;
  VkExtent2D mExtent = {};
  VkSwapchainKHR mSwapchain = VK_NULL_HANDLE;
  std::vector<VkImage> mSwapchainImages = {};
  VkSurfaceFormatKHR mSwapchainImageFormat = {};
  std::vector<VkImageView> mSwapchainImageViews = {};
  VkCommandPool mCommandPool = VK_NULL_HANDLE;
  VkDescriptorPool mImGuiPool = VK_NULL_HANDLE;
  FUInt32 mMaxFramesInFlight = 0;
  FUInt32 mCurrentFrame = 0;
  std::vector<VkSemaphore> mImageAvailableSemaphores = {};
  std::vector<VkSemaphore> mRenderFinishedSemaphores = {};
  std::vector<VkFence> mFrameInFlightFences = {};
};
