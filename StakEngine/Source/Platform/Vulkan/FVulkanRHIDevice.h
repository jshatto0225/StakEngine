#pragma once

#include "IRHIDevice.h"
#include "FVulkanRHIInstance.h"

#include <vulkan/vulkan.h>

#include <optional>
#include <set>

class FVulkanRHIDevice final : public IRHIDevice {
public:
  FVulkanRHIDevice(TRef<IRHIInstance> Instance, TRef<IWindow> Window);
  ~FVulkanRHIDevice();

public:
  inline const VkImageView &GetSwapchainImageView(FUInt32 Index) const { return mSwapchainImageViews[Index]; }
  inline const VkImage &GetSwapchainImage(FUInt32 Index) const { return mSwapchainImages[Index]; }
  inline const VkFormat &GetSwapchainImageFormat() const { return mSwapchainImageFormat.format; }
  inline VkDevice GetVkDevice() const { return mDevice; }
  inline FUInt32 GetGraphicsQueueFamilyIndex() const { return mGraphicsQueueFamily; }

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
};
