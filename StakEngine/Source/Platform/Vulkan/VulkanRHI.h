#pragma once

#include "RHI.h"

#include <vulkan/vulkan.h>
#include <vector>
#include <optional>

class FVulkanRHICommandContext;

class FVulkanRHI final : public FRHI {
public:
  FVulkanRHI();
  ~FVulkanRHI() override;

  FRHISamplerStateRef CreateSamplerState(const FRHISamplerStateInitializer &Initializer) override;
  FRHIRasterizerStateRef CreateRasterizerState(const FRHIRasterizerStateInitializer &Initializer) override;
  FRHIDepthStencilStateRef CreateDepthStencilState(const FRHIDepthStencilStateInitializer &Initializer) override;
  FRHIBlendStateRef CreateBlendState(const FRHIBlendStateInitializer &Initializer) override;
  FRHIVertexDeclarationRef CreateVertexDeclaration(const FRHIVertexDeclarationElementList &Elements) override;

  FRHIPixelShaderRef CreatePixelShader() override;
  FRHIVertexShaderRef CreateVertexShader() override;
  FRHIGeometryShaderRef CreateGeometryShader() override;

  FRHIBufferRef CreateBuffer(const FRHIBufferDescription &Description, ERHIAccess Access) override;
  FRHIUniformBufferRef CreateUniformBuffer(const void *InitialContents, const FRHIUniformBufferLayout &Layout, ERHIUniformBufferUsage Usage) override;
  FRHIStagingBufferRef CreateStagingBuffer() override;

  FRHITextureRef CreateTexture(const FRHITextureDescription &Description) override;

  FRHIGraphicsPipelineStateRef CreateGraphicsPipelineState(const FRHIGraphicsPipelineStateInitializer &Initializer) override;

  FRHIShaderResourceViewRef CreateShaderResourceView(FRHIViewableResourceRef Resource, const FRHIViewDescription &Description) override;
  FRHIUnorderedAccessViewRef CreateUnorderedAccessView(FRHIViewableResourceRef Resource, const FRHIViewDescription &Description) override;

  void InitImGui() override;
  void ShutdownImGui() override;
  void ImGuiNewFrame() override;
  void SubmitImGuiDrawData(ImDrawData *DrawData) override;
  
  inline void FramebufferResized() override { mFramebufferResized = true; }

  void BeginDrawing() override;
  void EndDrawing() override;

  void WaitForGPUIdle() override;
  
  IRHICommandContext *GetCommandContext() override;
  
public:
  inline VkSwapchainKHR GetVkSwapchain() const { return mSwapchain; }
  inline VkSemaphore GetVkRenderFinishedSemaphore() const { return mRenderFinishedSemaphores[mCurrentFrame]; }
  inline VkQueue GetVkPresentQueue() const { return mPresentQueue; }

private:
  struct VulkanQueueFamilies {
    std::optional<FUInt32> GraphicsFamily;
    std::optional<FUInt32> PresentFamily;
  };

  VulkanQueueFamilies FindQueueFamilies(VkPhysicalDevice Device);

  struct VulkanSwapchainSupport {
    VkSurfaceCapabilitiesKHR Capabilities;
    std::vector<VkSurfaceFormatKHR> Formats;
    std::vector<VkPresentModeKHR> PresentModes;
  };

  VulkanSwapchainSupport GetSwapchainSupport(VkPhysicalDevice Device);

  void CreateSwapchain();
  void RecreateSwapchain();
  void CreateImageViews();
  VkImageView CreateImageView(VkImage Image, VkFormat Format);
  void CreateSyncObjects();
  
private:
  FVulkanRHICommandContext *mCommandContext;

private:
  VkInstance mInstance = VK_NULL_HANDLE;
  VkDevice mDevice = VK_NULL_HANDLE;
  VkPhysicalDevice mGPU = VK_NULL_HANDLE;
  VkQueue mPresentQueue = VK_NULL_HANDLE;
  FUInt32 mPresentQueueIndex = UINT32_MAX;
  VkQueue mGraphicsQueue = VK_NULL_HANDLE;
  FUInt32 mGraphicsQueueIndex = UINT32_MAX;
  std::vector<VkCommandBuffer> mCommandBuffers;
  std::vector<VkImage> mSwapchainImages;
  VkDebugUtilsMessengerEXT mDebugMessenger = VK_NULL_HANDLE;
  VkSurfaceKHR mSurface = VK_NULL_HANDLE;
  VkExtent2D mSwapchainExtent = {};
  VkSurfaceFormatKHR mSwapchainImageFormat = {};
  VkSwapchainKHR mSwapchain = VK_NULL_HANDLE;
  std::vector<VkImageView> mSwapchainImageViews;
  std::vector<VkSemaphore> mRenderFinishedSemaphores;
  std::vector<VkSemaphore> mImageAvailableSemaphores;
  std::vector<VkFence> mInFlightFences;
  VkDescriptorPool mImGuiPool = VK_NULL_HANDLE;
  VkCommandPool mCommandPool = VK_NULL_HANDLE;
  FUInt32 mCurrentFrame = 0;
  // TODO
  FUInt32 mImageIndex = 0;
  FBool mFramebufferResized = false;

private:
  const std::vector<const char *> VALIDATION_LAYERS = {
    "VK_LAYER_KHRONOS_validation"
  };

  const std::vector<const char *> DEVICE_EXTENSIONS = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
  };

  const FUInt32 MAX_FRAMES_IN_FLIGHT = 2;

#ifdef SK_DEBUG
  const FBool ENABLE_VALIDATION_LAYERS = true;
#else
  const FBool ENABLE_VALIDATION_LAYERS = false;
#endif
};