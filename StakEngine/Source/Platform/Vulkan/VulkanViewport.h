#pragma once

#include "RHIViewport.h"

#include "VulkanRHI.h"

struct FVulkanTexture;

struct FVulkanViewport : public IRHIViewport {
    FVulkanViewport(VkInstance Instance, VkPhysicalDevice GPU, VkDevice Device, void *WindowHandle);

    bool Init() override;
    void Shutdown() override;

    bool PrepareFrame(FUInt32 FrameIndex);
    bool PresentFrame(FUInt32 FrameIndex);
    bool CreateSwapchain();
    bool RecreateSwapchain();

    VkSemaphore ImageAvailableSemaphores[MAX_FRAMES_IN_FLIGHT] = {};
    VkSemaphore RenderFinishedSemaphores[MAX_FRAMES_IN_FLIGHT] = {};

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

    void *WindowHandle = nullptr;
};
