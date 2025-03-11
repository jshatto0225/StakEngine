#pragma once

#include "RHIViewport.h"

#include "VulkanRHI.h"

class FVulkanTexture;

class FVulkanViewport : public IRHIViewport {
public:
    FVulkanViewport(VkInstance Instance, VkPhysicalDevice GPU, VkDevice Device, void *WindowHandle);

    TRef<IRHITexture> GetCurrentBackbuffer() override;

    bool Init() override;
    void Shutdown() override;

    void OnFramebufferResize() override;

public:
    bool PrepareFrame(FUInt32 FrameIndex);
    bool PresentFrame(FUInt32 FrameIndex);
    bool CreateSwapchain();
    bool RecreateSwapchain();
    FUInt32 GetCurrentImageIndex() { return ImageIndex; }
    FUInt32 GetPresentQueueIndex() { return PresentQueueIndex; }
    FUInt32 GetMinImageCount() { return MinImageCount; }
    FUInt32 GetImageCount() { return ImageCount; }

public:
    VkSemaphore ImageAvailableSemaphores[MAX_FRAMES_IN_FLIGHT] = {};
    VkSemaphore RenderFinishedSemaphores[MAX_FRAMES_IN_FLIGHT] = {};

private:
    VkDevice Device = nullptr;
    VkSurfaceKHR Surface = nullptr;
    VkSwapchainKHR Swapchain = nullptr;
    VkInstance Instance  = nullptr;
    VkPhysicalDevice GPU = nullptr;
    FSInt32 PresentQueueIndex = -1;
    VkQueue PresentQueue = nullptr;
    FUInt32 ImageIndex = 0;

    FUInt32 ImageCount = 0;
    FUInt32 MinImageCount = 0;

    bool FramebufferResized = false;

    void *WindowHandle = nullptr;

    std::vector<TRef<FVulkanTexture>> Backbuffers;
};
