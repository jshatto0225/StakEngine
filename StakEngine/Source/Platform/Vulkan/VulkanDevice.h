#pragma once

#include "RHIDevice.h"
#include "Window.h"

#include "VulkanRHI.h"
#include "RHITexture.h"
#include "RHIShader.h"
#include "VulkanTexture.h"

#include <vulkan/vulkan.h>

class FVulkanDevice : public IRHIDevice {
public:
    FVulkanDevice(TRef<IWindow> Window, VkInstance Instance);

    void Submit(FRHICommandContext &CommandContext) override;

    void ImGuiNewFrame() override;
    void ShutdownImGui() override;
    void InitImGui() override;

    void PrepareFrame() override;
    void PresentFrame() override;
    void WaitForGPUIdle() override;

    FUInt32 GetGraphicsQueueIndex() override;
    FUInt32 GetPresentQueueIndex() override;

    TRef<IRHICommandContext> CreateCommandContext() override;

    TRef<IRHIBuffer> CreateBuffer(const FRHIBufferDescription &Description) override;

    TRef<IRHIShader> CreateShader(const FRHIShaderDescription &Description) override;

    TRef<IRHIPipelineLayout> CreatePipelineLayout(const FRHIPipelineLayoutDescription &Description) override;

    TRef<IRHIDescriptorSetLayout> CreateDescriptorSetLayout(const FRHIDescriptorSetLayoutDescription &Description) override;

    TRef<IRHIPipeline> CreatePipeline(const FRHIGraphicsPipelineStateDescription &Description) override;

    FRHITexture *GetBackbuffer() override;

    void Shutdown() override;

public:
    void CreateSwapchain();
    void RecreateSwapchain();

    VkCommandBuffer BeginOneTimeVulkanCommandBuffer();
    void EndOneTimeVulkanCommandBuffer(VkCommandBuffer CommandBuffer);
    void CopyVulkanBuffer(VkBuffer Src, VkBuffer Dst, FUInt32 Size);
    void CreateVulkanBuffer(FUInt32 Size, VkBufferUsageFlags Usage, VkMemoryPropertyFlags Properties, VkBuffer *Buffer, VkDeviceMemory *Memory);
    FUInt32 FindVulkanMemoryType(FUInt32 Filter, VkMemoryPropertyFlags Flags);

public:
    FUInt32 GetCurrentFrameIndex();
    FUInt32 GetCurrentImageIndex();
    VkDevice GetVulkanDevice();

private:
    bool Initialized = true;

    TRef<IWindow> Window = nullptr;
    VkInstance Instance = VK_NULL_HANDLE;

    VkDevice Device = VK_NULL_HANDLE;
    VkPhysicalDevice GPU = VK_NULL_HANDLE;
    VkSurfaceKHR Surface = VK_NULL_HANDLE;
    VkSwapchainKHR Swapchain = VK_NULL_HANDLE;
    VkQueue PresentQueue = VK_NULL_HANDLE;
    VkQueue GraphicsQueue = VK_NULL_HANDLE;
    FUInt32 GraphicsQueueIndex = UINT32_MAX;
    FUInt32 PresentQueueIndex = UINT32_MAX;
    FUInt32 CurrentFrame = 0;
    FUInt32 ImageIndex = 0;
    VkSemaphore RenderFinishedSemaphores[MAX_FRAMES_IN_FLIGHT] = {};
    VkSemaphore ImageAvailableSemaphores[MAX_FRAMES_IN_FLIGHT] = {};
    VkFence InFlightFences[MAX_FRAMES_IN_FLIGHT] = {};
    bool FramebufferResized = false;
    VkDescriptorPool ImGuiPool = VK_NULL_HANDLE;

    TRef<FVulkanTexture> Backbuffer = nullptr;
    FRHITexture BackbufferTexture;

    // NOTE: For one time commands
    VkCommandPool CommandPool = VK_NULL_HANDLE;
};
