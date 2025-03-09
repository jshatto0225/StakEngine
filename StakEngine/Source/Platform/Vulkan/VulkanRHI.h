#pragma once

#include "Types.h"
#include "RHI.h"
#include "Log.h"
#include "RHIResource.h"
#include "RHIDescriptorSetLayout.h"
#include "RHIShader.h"
#include "RHIPipeline.h"

#include <vulkan/vulkan.h>
#include <vector>
#include <optional>

class FVulkanViewport;

#define CHECK_VK_ERR(Err, Message)              \
    if (Err != VK_SUCCESS) {                    \
        SK_LOG_ERROR(Message);                  \
    }

#define MAX_FRAMES_IN_FLIGHT 2

const std::vector<const char*> VALIDATION_LAYERS = {
    "VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> DEVICE_EXTENSIONS = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

#ifdef SK_DEBUG
const FBool ENABLE_VALIDATION_LAYERS = true;
#else
const FBool ENABLE_VALIDATION_LAYERS = false;
#endif

struct VulkanQueueFamilies {
    std::optional<FUInt32> GraphicsFamily;
    std::optional<FUInt32> PresentFamily;
};

struct VulkanSwapchainSupport {
    VkSurfaceCapabilitiesKHR Capabilities;
    std::vector<VkSurfaceFormatKHR> Formats;
    std::vector<VkPresentModeKHR> PresentModes;
};

bool GetSwapchainSupport(VulkanSwapchainSupport *Out, VkPhysicalDevice Device, VkSurfaceKHR Surface);

bool VulkanCreateImageView(VkImageView *Out, VkDevice Device, VkImage Image, VkFormat Format);

VkIndexType GetVulkanIndexType(ERHIFormat Format);
VkDescriptorType GetVulkanDescriptorType(ERHIDescriptorType Type);
VkShaderStageFlags GetVulkanShaderStageFlags(std::vector<ERHIShaderType> Types);
VkShaderStageFlagBits GetVulkanShaderStage(ERHIShaderType Type);
VkVertexInputRate GetVulkanVertexInputRate(ERHIVertexInputRate InputRate);
VkFormat GetVulkanFormat(ERHIFormat Format);
VkFormat GetVulkanDepthFormat(ERHIFormat Format);
VkFormat GetVulkanStencilFormat(ERHIFormat Format);
ERHIFormat GetRHIFormat(VkFormat Format);

enum class EVulkanQueue {
    GRAPHICS,
    PRESENT,
    ANY,
};

VkAccessFlags GetVulkanAccessMask(ERHIResourceState State);
VkImageLayout GetVulkanImageLayout(ERHIResourceState State);
EVulkanQueue GetVulkanQueue(ERHIResourceState State);
VkPipelineStageFlags GetVulkanPipelineStageMask(ERHIResourceState State);
FSInt32 FindPresentQueueIndex(VkPhysicalDevice GPU, VkSurfaceKHR Surface);

class FVulkanRHI : public IRHI {
public:
    bool Init() override;
    void Shutdown() override;
    void ImGuiNewFrame() override;
    void ShutdownImGui() override;
    void InitImGui() override;
    bool WaitForGPUIdle() override;
    bool Submit(TRef<IRHICommandContext> Context) override;
    TRef<IRHICommandContext> CreateCommandContext() override;
    TRef<IRHIShader> CreateShader() override;
    TRef<IRHIBuffer> CreateBuffer() override;
    TRef<IRHIPipelineLayout> CreatePipelineLayout() override;
    TRef<IRHIDescriptorSetLayout> CreateDescriptorSetLayout() override;
    TRef<IRHIPipeline> CreatePipeline() override;
    bool PrepareFrame() override;
    bool PresentFrame() override;
    bool SetActiveViewport(TRef<IRHIViewport> Viewport) override;
    TRef<IRHITexture> GetCurrentBackbuffer() override;
    TRef<IRHIViewport> CreateViewport(void *WindowHandle) override;

public:
    VkCommandBuffer BeginOneTimeCommandBuffer();
    bool EndOneTimeCommandBuffer(VkCommandBuffer CommandBuffer);
    bool CopyBuffer(VkBuffer Src, VkBuffer Dst, FUInt32 Size);
    bool CreateBuffer(FUInt32 Size, VkBufferUsageFlags Usage, VkMemoryPropertyFlags Properties, VkBuffer *Buffer, VkDeviceMemory *Memory);
    FUInt32 FindMemoryType(FUInt32 Filter, VkMemoryPropertyFlags Flags);
    FUInt32 GetCurrentFrameIndex();
    inline FUInt32 GetGraphicsQueueIndex() { return static_cast<FUInt32>(GraphicsQueueIndex); }
    FUInt32 GetCurrentImageIndex();
    FUInt32 GetActivePresentQueueIndex();

private:
    VkInstance Instance = nullptr;
    VkDevice Device = nullptr;
    VkPhysicalDevice GPU = nullptr;
    VkDebugUtilsMessengerEXT DebugMessenger = nullptr;

    VkQueue GraphicsQueue = nullptr;
    FSInt32 GraphicsQueueIndex = -1;

    FUInt32 CurrentFrame = 0;

    VkCommandPool CommandPool = nullptr;

    VkFence InFlightFences[MAX_FRAMES_IN_FLIGHT] = {};

    VkDescriptorPool ImGuiPool = nullptr;

    TRef<FVulkanViewport> ActiveViewport;
};
