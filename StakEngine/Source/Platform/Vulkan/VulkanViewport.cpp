#include "VulkanViewport.h"

#include "VulkanTexture.h"

#include "Platform.h"

#ifdef SK_GLFW
#include "GLFWWindow.h"
#endif

extern IRHI *GRHI;

FVulkanViewport::FVulkanViewport(VkInstance Instance, VkPhysicalDevice GPU, VkDevice Device, void *WindowHandle) : Device(Device), Instance(Instance), GPU(GPU), WindowHandle(WindowHandle) {}

bool FVulkanViewport::Init() {
#ifdef SK_GLFW
    if (glfwCreateWindowSurface(Instance, (GLFWwindow *) WindowHandle, nullptr, &Surface) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to create window surface");
        return false;
    }
#endif

    VkSemaphoreCreateInfo SemaphoreInfo = {};
    SemaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    for (FUInt32 Index = 0; Index < MAX_FRAMES_IN_FLIGHT; Index++) {
        if (vkCreateSemaphore(Device, &SemaphoreInfo, nullptr, &ImageAvailableSemaphores[Index]) != VK_SUCCESS) {
            SK_LOG_ERROR("Failed to create vulkan semaphore");
            return false;
        }
        if (vkCreateSemaphore(Device, &SemaphoreInfo, nullptr, &RenderFinishedSemaphores[Index]) != VK_SUCCESS) {
            SK_LOG_ERROR("Failed to create vulkan semaphore");
            return false;
        }
    }

    if (!CreateSwapchain()) {
        SK_LOG_ERROR("Failed to create swapchain");
        return false;
    }

    vkGetDeviceQueue(Device, PresentQueueIndex, 0, &PresentQueue);

    return true;
}

bool FVulkanViewport::CreateSwapchain() {
    VulkanSwapchainSupport Support;
    if (!GetSwapchainSupport(&Support, GPU, Surface)) {
        SK_LOG_ERROR("Failed to get swapchain support");
        return false;
    }

    VkSurfaceFormatKHR Format = {};
    for (auto &F : Support.Formats) {
        if (F.format == VK_FORMAT_B8G8R8A8_SRGB && F.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            Format = F;
            break;
        }
    }

    VkPresentModeKHR PresentMode = {};
    for (auto &Mode : Support.PresentModes) {
        if (Mode == VK_PRESENT_MODE_MAILBOX_KHR) {
            PresentMode = Mode;
            break;
        }
    }

    VkExtent2D SwapchainExtent = {};

    if (Support.Capabilities.currentExtent.width != UINT32_MAX) {
        SwapchainExtent = Support.Capabilities.currentExtent;
    } else {
#ifdef SK_GLFW
        FSInt32 Width;
        FSInt32 Height;
        glfwGetFramebufferSize((GLFWwindow *) WindowHandle, &Width, &Height);
#endif

        VkExtent2D Extent = {
            static_cast<FUInt32>(Width),
            static_cast<FUInt32>(Height)
        };

        Extent.width = std::clamp(Extent.width, Support.Capabilities.minImageExtent.width, Support.Capabilities.maxImageExtent.width);
        Extent.height = std::clamp(Extent.height, Support.Capabilities.minImageExtent.height, Support.Capabilities.maxImageExtent.height);

        SwapchainExtent = Extent;
    }

    ImageCount = Support.Capabilities.minImageCount + 1;

    if (Support.Capabilities.maxImageCount > 0 && ImageCount > Support.Capabilities.maxImageCount) {
        ImageCount = Support.Capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR SwapchainInfo = {};
    SwapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    SwapchainInfo.surface = Surface;
    SwapchainInfo.minImageCount = ImageCount;
    SwapchainInfo.imageFormat = Format.format;
    SwapchainInfo.imageColorSpace = Format.colorSpace;
    SwapchainInfo.imageExtent = SwapchainExtent;
    SwapchainInfo.imageArrayLayers = 1;
    SwapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    MinImageCount = ImageCount;

    PresentQueueIndex = FindPresentQueueIndex(GPU, Surface);

    auto RHI = reinterpret_cast<FVulkanRHI *>(GRHI);
    assert(RHI->GetGraphicsQueueIndex() == PresentQueueIndex);

    SwapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    SwapchainInfo.preTransform = Support.Capabilities.currentTransform;
    SwapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    SwapchainInfo.presentMode = PresentMode;
    SwapchainInfo.clipped = VK_TRUE;
    SwapchainInfo.oldSwapchain = VK_NULL_HANDLE;
    if (vkCreateSwapchainKHR(Device, &SwapchainInfo, nullptr, &Swapchain) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to create swapchian");
        return false;
    }

    std::vector<VkImage> SwapchainImages = {};
    SwapchainImages.resize(ImageCount);

    vkGetSwapchainImagesKHR(Device, Swapchain, &ImageCount, SwapchainImages.data());
    
    if (Backbuffers.size() != ImageCount) {
        Backbuffers.clear();
        Backbuffers.resize(ImageCount);
        for (auto &Backbuffer : Backbuffers) {
            Backbuffer = TCreateRef<FVulkanTexture>(Device, GPU);
        }
    }

    for (FUInt32 BackbufferIndex = 0; BackbufferIndex < ImageCount; BackbufferIndex++) {
        if (!Backbuffers[BackbufferIndex]->Init(SwapchainImages[BackbufferIndex], SwapchainExtent, Format.format)) {
            SK_LOG_ERROR("Failed to initialize backbuffer");
            return false;
        }
    }

    return true;
}

bool FVulkanViewport::PrepareFrame(FUInt32 FrameIndex) {
    VkResult Err = vkAcquireNextImageKHR(Device, Swapchain, UINT64_MAX, ImageAvailableSemaphores[FrameIndex], VK_NULL_HANDLE, &ImageIndex);

    if (Err == VK_ERROR_OUT_OF_DATE_KHR) {
        if (!RecreateSwapchain()) {
            SK_LOG_ERROR("Failed to recreate swapchain");
            return false;
        }
        return PrepareFrame(FrameIndex);
    } 
    
    if (Err != VK_SUCCESS && Err != VK_SUBOPTIMAL_KHR) {
        SK_LOG_ERROR("Failed to get next swapchain image");
        return false;
    }

    return true;
}

bool FVulkanViewport::PresentFrame(FUInt32 FrameIndex) {
    VkPresentInfoKHR PresentInfo = {};
    PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    PresentInfo.swapchainCount = 1;
    PresentInfo.pSwapchains = &Swapchain;
    PresentInfo.waitSemaphoreCount = 1;
    PresentInfo.pWaitSemaphores = &RenderFinishedSemaphores[FrameIndex];
    PresentInfo.pImageIndices = &ImageIndex;
    VkResult Err = vkQueuePresentKHR(PresentQueue, &PresentInfo);

    if (Err == VK_ERROR_OUT_OF_DATE_KHR || Err == VK_SUBOPTIMAL_KHR || FramebufferResized) {
        if (!RecreateSwapchain()) {
            SK_LOG_ERROR("Failed to recreate swapchian");
            return false;
        }
    } else if (Err != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to present");
        return false;
    }

    return true;
}

TRef<IRHITexture> FVulkanViewport::GetCurrentBackbuffer() {
    return Backbuffers[ImageIndex];
}

void FVulkanViewport::Shutdown() {
    for (auto &Backbuffer : Backbuffers) {
        Backbuffer->Shutdown();
    }

    vkDestroySwapchainKHR(Device, Swapchain, nullptr);

    vkDestroySurfaceKHR(Instance, Surface, nullptr);

    for (VkSemaphore Semaphore : ImageAvailableSemaphores) {
        vkDestroySemaphore(Device, Semaphore, nullptr);
    }
}

bool FVulkanViewport::RecreateSwapchain() {
#ifdef SK_GLFW
    FSInt32 Width = 0;
    FSInt32 Height = 0;
    auto Window = (GLFWwindow *) WindowHandle;

    glfwGetFramebufferSize(Window, &Width, &Height);

    while (Width == 0 || Height == 0) {
        glfwPollEvents();
        glfwGetFramebufferSize(Window, &Width, &Height);
    }
#endif

    if (!RHIWaitForGPUIdle()) {
        SK_LOG_ERROR("Failed to wait for gpu before swapchain recreate");
        return false;
    }

    for (auto &Backbuffer : Backbuffers) {
        Backbuffer->Shutdown();
    }

    vkDestroySwapchainKHR(Device, Swapchain, nullptr);

    if (!CreateSwapchain()) {
        SK_LOG_ERROR("Failed to crate swapchain");
        return false;
    }

    FramebufferResized = false;

    return true;
}

void FVulkanViewport::OnFramebufferResize() {
    FramebufferResized = true;
}
