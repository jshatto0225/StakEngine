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

VulkanSwapchainSupport GetSwapchainSupport(VkPhysicalDevice Device, VkSurfaceKHR Surface);

VkImageView VulkanCreateImageView(VkDevice Device, VkImage Image, VkFormat Format);

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
    FVulkanRHI();
    ~FVulkanRHI();

    void ImGuiNewFrame() override;
    void ShutdownImGui() override;
    void InitImGui() override;
    void WaitForGPUIdle() override;
    void Submit(TRef<IRHICommandContext> Context) override;
    TRef<IRHICommandContext> CreateCommandContext() override;
    TRef<IRHIShader> CreateShader(const FRHIShaderDescription &Description) override;
    TRef<IRHIBuffer> CreateBuffer(const FRHIBufferDescription &Description) override;
    TRef<IRHIPipelineLayout> CreatePipelineLayout(const FRHIPipelineLayoutDescription &Description) override;
    TRef<IRHIDescriptorSetLayout> CreateDescriptorSetLayout(const FRHIDescriptorSetLayoutDescription &Description) override;
    TRef<IRHIPipeline> CreatePipeline(const FRHIGraphicsPipelineStateDescription &Description) override;
    void PrepareFrame() override;
    void PresentFrame() override;
    void SetActiveViewport(TRef<IRHIViewport> Viewport) override;
    TRef<IRHITexture> GetCurrentBackbuffer() override;
    TRef<IRHIViewport> CreateViewport(void *WindowHandle) override;

public:
    VkCommandBuffer BeginOneTimeCommandBuffer();
    void EndOneTimeCommandBuffer(VkCommandBuffer CommandBuffer);
    void CopyBuffer(VkBuffer Src, VkBuffer Dst, FUInt32 Size);
    void CreateBuffer(FUInt32 Size, VkBufferUsageFlags Usage, VkMemoryPropertyFlags Properties, VkBuffer *Buffer, VkDeviceMemory *Memory);
    FUInt32 FindMemoryType(FUInt32 Filter, VkMemoryPropertyFlags Flags);
    FUInt32 GetCurrentFrameIndex();
    FUInt32 GetGraphicsQueueIndex() { return static_cast<FUInt32>(GraphicsQueueIndex); }
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
