#pragma once

#include "Types.h"
#include "RHI.h"
#include "Log.h"

#include <vulkan/vulkan.h>
#include <vector>
#include <optional>

struct FVulkanViewport;

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

struct VulkanSwapchainSupport {
    VkSurfaceCapabilitiesKHR Capabilities;
    std::vector<VkSurfaceFormatKHR> Formats;
    std::vector<VkPresentModeKHR> PresentModes;
};

enum class EVulkanQueue {
    GRAPHICS,
    PRESENT,
    ANY,
};

struct FVulkanTexture {
    bool SwapchainTexture;
    VkImage Image;
    VkDeviceMemory ImageMemory;
    VkImageView ImageView;
    VkSampler Sampler;
    VkImageSubresourceRange *SubresourceRanges;
    FUInt32 SubresourceRangeCount;
    VkFormat Format;
    VkExtent2D Extent;
    FUInt32 ImageCount;
    VkDescriptorSet ImGuiDescriptorSet;
    FRHIRenderArea RenderArea;

    bool IsShaderResource;
    bool IsImGuiCompatible;
};

struct FVulkanSwapchain {
    FUInt32 ImageCount;
    FUInt32 MinImageCount;
    bool FramebufferResized;
    FVulkanTexture *Images;
    FUInt32 ImageIndex;
    VkFormat Format;
    VkSemaphore ImageAvailableSemaphores[MAX_FRAMES_IN_FLIGHT];
    VkSemaphore RenderFinishedSemaphores[MAX_FRAMES_IN_FLIGHT];
    VkQueue PresentQueue;
    FUInt32 PresentQueueIndex;
    VkSwapchainKHR Swapchain;
    FHandle WindowHandle;
    VkSurfaceKHR Surface;
};

struct FVulkanCommandList {
    VkCommandBuffer Buffers[MAX_FRAMES_IN_FLIGHT];
};

struct FVulkanShader {
    VkShaderStageFlagBits Stage;
    VkShaderModule Shader;
};

struct FVulkanBuffer {
    ERHIBufferType Type;
    FUInt32 Size;
    FUInt32 ElementCount;
    FRHIBufferLayout Layout;
    bool UseStagingBuffer;
    VkBuffer Buffer;
    VkBuffer StagingBuffer;
    VkDeviceMemory BufferMemory;
    VkDeviceMemory StagingBufferMemory;
    void *MappedData;
};

struct FVulkanPipelineLayout {
    VkPipelineLayout Layout;
};

struct FVulkanDescriptorSetLayout {
    VkDescriptorSetLayout Layout;
};

struct FVulkanPipeline {
    VkPipelineBindPoint BindPoint;
    VkPipeline Pipeline;
};

bool VulkanGetSwapchainSupport(VulkanSwapchainSupport *Out, VkPhysicalDevice Device, VkSurfaceKHR Surface);
FUInt32 VulkanFindMemoryType(FUInt32 Filter, VkMemoryPropertyFlags Flags);
VkAccessFlags VulkanGetAccessMask(ERHIResourceState State);
VkImageLayout VulkanGetImageLayout(ERHIResourceState State);
EVulkanQueue VulkanGetQueue(ERHIResourceState State);
VkPipelineStageFlags VulkanGetPipelineStageMask(ERHIResourceState State);
VkIndexType VulkanGetIndexType(ERHIFormat Format);
VkDescriptorType VulkanGetDescriptorType(ERHIDescriptorType Type);
VkShaderStageFlags VulkanGetShaderStageFlags(std::vector<ERHIShaderType> Types);
VkShaderStageFlagBits VulkanGetShaderStage(ERHIShaderType Type);
VkVertexInputRate VulkanGetVertexInputRate(ERHIVertexInputRate InputRate);
VkFormat VulkanGetFormat(ERHIFormat Format);
VkFormat VulkanGetDepthFormat(ERHIFormat Format);
ERHIFormat VulkanGetRHIFormat(VkFormat Format);
VkFormat VulkanGetStencilFormat(ERHIFormat Format);
FSInt32 FindPresentQueueIndex(VkSurfaceKHR Surface);
FUInt32 VulkanFindMemoryType(FUInt32 Filter, VkMemoryPropertyFlags Flags);
VkCommandBuffer VulkanBeginOneTimeCommandBuffer();
bool VulkanEndOneTimeCommandBuffer(VkCommandBuffer CommandBuffer);
bool VulkanCopyBuffer(VkBuffer Src, VkBuffer Dst, FUInt32 Size);
bool VulkanCreateBuffer(FUInt32 Size, VkBufferUsageFlags Usage, VkMemoryPropertyFlags Properties, VkBuffer *Buffer, VkDeviceMemory *Memory);
bool VulkanRecreateSwapchain(FVulkanSwapchain *Swapchain);
bool VulkanCreateSwapchainTexture(FVulkanTexture *Out, VkImage Image, VkExtent2D Extent, VkFormat Format);
bool VulkanCreateImageView(VkImageView *Out, VkDevice Device, VkImage Image, VkFormat Format);
bool VulkanCreateImage(VkDevice Device, VkPhysicalDevice GPU, FUInt32 Width, FUInt32 Height, VkFormat Format, VkImageTiling Tiling, VkImageUsageFlags Flags, VkMemoryPropertyFlags Properties, VkImage *OutImage, VkDeviceMemory *OutImageMemory);
bool VulkanCreateTextureSampler(VkSampler *Out, VkDevice Device, VkPhysicalDevice GPU);
void VulkanTransitionBarrier(FVulkanCommandList *List, FRHITransitionBarrier *Barrier);

// RHI:
FRHI VulkanCreateRHI();

bool VulkanInit();
void VulkanShutdown();

void VulkanImGuiNewFrame();
void VulkanShutdownImGui();
void VulkanInitImGui(FRHIResourceHandle Swapchain);

bool VulkanWaitForGPUIdle();

bool VulkanPrepareFrame(FRHIResourceHandle Swapchain);
bool VulkanPresentFrame();
bool VulkanSubmit(FRHIResourceHandle CommandContext);

void VulkanAddTextureToImGuiWindow(FRHIResourceHandle Texture);

FRHIResourceHandle VulkanCreateTexture(FRHITextureDescription *Description);
void VulkanDestroyTexture(FRHIResourceHandle *Resource);

FRHIResourceHandle VulkanCreateSwapchain(FWindow *Window);
void VulkanDestroySwapchain(FRHIResourceHandle *Resource);

FRHIResourceHandle VulkanCreateCommandList(FRHICommandListDescription *Description);
void VulkanDestroyCommandList(FRHIResourceHandle *Resource);

FRHIResourceHandle VulkanCreateShader(FRHIShaderDescription *Description);
void VulkanDestroyShader(FRHIResourceHandle *Resource);

FRHIResourceHandle VulkanCreateVertexBuffer(FRHIVertexBufferDescription *Description);
FRHIResourceHandle VulkanCreateIndexBuffer(FRHIIndexBufferDescription *Description);
void VulkanDestroyBuffer(FRHIResourceHandle *Resource);

FRHIResourceHandle VulkanCreatePipelineLayout(FRHIPipelineLayoutDescription *Description);
void VulkanDestroyPipelineLayout(FRHIResourceHandle *Resource);

FRHIResourceHandle VulkanCreateDescriptorSetLayout(FRHIDescriptorSetLayoutDescription *Description);
void VulkanDestroyDescriptorSetLayout(FRHIResourceHandle *Resource);

FRHIResourceHandle VulkanCreateGraphicsPipeline(FRHIGraphicsPipelineStateDescription *Description);
void VulkanDestroyPipeline(FRHIResourceHandle *Resource);

void VulkanSetBufferData(FRHIResourceHandle Buffer, void *Data, FUInt32 Size);
void VulkanMapBufferData(FRHIResourceHandle Buffer, void **Data, FUInt32 Size);
bool VulkanSendBufferToGPU(FRHIResourceHandle Buffer);

FRHIResourceHandle VulkanGetCurrentSwapchainTexture(FRHIResourceHandle Swapchain);
ERHIFormat VulkanGetTextureFormat(FRHIResourceHandle Texture);
FRHIRenderArea VulkanGetTextureRenderArea(FRHIResourceHandle Texture);

bool VulkanBeginCommandList(FRHIResourceHandle CommandList);
bool VulkanEndCommandList(FRHIResourceHandle CommandList);

void VulkanCmdRenderImGuiDrawData(FRHIResourceHandle CommandList, ImDrawData *DrawData);
void VulkanCmdResourceBarrier(FRHIResourceHandle CommandList, FRHIResourceBarrier *Barrier);
void VulkanCmdSetRenderTarget(FRHIResourceHandle CommandList, FRHIResourceHandle Target, FRHIRenderArea *RenderArea);
void VulkanCmdUnsetRenderTarget(FRHIResourceHandle CommandList);
void VulkanCmdBindVertexBuffer(FRHIResourceHandle CommandList, FRHIResourceHandle Buffer, FUInt32 FirstVertex);
void VulkanCmdBindIndexBuffer(FRHIResourceHandle CommandList, FRHIResourceHandle Buffer);
void VulkanCmdDrawIndexed(FRHIResourceHandle CommandList, FUInt32 IndexCount, FUInt32 InstanceCount, FUInt32 FirstIndex, FSInt32 VertexOffset, FUInt32 FirstInstance);
void VulkanCmdDrawInstanced(FRHIResourceHandle CommandList, FUInt32 VertexCount, FUInt32 InstanceCount, FUInt32 FirstVertex, FUInt32 FirstInstance);
void VulkanCmdSetViewport(FRHIResourceHandle CommandList, FFloat X, FFloat Y, FFloat Width, FFloat Height, FFloat MinDepth, FFloat MaxDepth);
void VulkanCmdSetScissor(FRHIResourceHandle CommandList, FSInt32 X, FSInt32 Y, FUInt32 Width, FUInt32 Height);
void VulkanCmdBindPipeline(FRHIResourceHandle CommandList, FRHIResourceHandle Pipeline);

void VulkanNotifySwapchainOfResize(FRHIResourceHandle Swapchain);