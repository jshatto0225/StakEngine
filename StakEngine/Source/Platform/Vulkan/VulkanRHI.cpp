#include "VulkanRHI.h"

#include "Log.h"
#include "Engine.h"

#include <backends/imgui_impl_vulkan.h>

#include <set>
#include <string>
#include <fstream>

#ifdef SK_GLFW
#include <glfw/glfw3.h>
#endif

struct VulkanData {
    VkInstance Instance;
    VkDevice Device;
    VkPhysicalDevice GPU;
    VkDebugUtilsMessengerEXT DebugMessenger;
    VkQueue GraphicsQueue;
    FSInt32 GraphicsQueueIndex;
    FUInt32 CurrentFrame;
    VkCommandPool CommandPool;
    VkFence InFlightFences[MAX_FRAMES_IN_FLIGHT];
    VkDescriptorPool ImGuiPool;
    FVulkanSwapchain *ActiveSwapchain;
};

static VulkanData VulkanContext = {};

VKAPI_ATTR VkBool32 VKAPI_CALL VulkanMessenger(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT *callback_data, void *user_data) {
    switch (severity) {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
        SK_LOG_ERROR(callback_data->pMessage);
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
        SK_LOG_INFO(callback_data->pMessage);
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
        SK_LOG_INFO(callback_data->pMessage);
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        SK_LOG_WARN(callback_data->pMessage);
        break;
    default:
        break;
    }

    return VK_FALSE;
}

VkResult VulkanCreateDebugMessenger(VkInstance Instance, const VkDebugUtilsMessengerCreateInfoEXT *Info, const VkAllocationCallbacks *Allocator, VkDebugUtilsMessengerEXT *Messenger) {
    auto Func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(Instance, "vkCreateDebugUtilsMessengerEXT");
    if (Func != nullptr) {
        return Func(Instance, Info, Allocator, Messenger);
    } else {
        SK_LOG_ERROR("Extension not present");
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void VulkanDestroyDebugMessenger(VkInstance Instance, VkDebugUtilsMessengerEXT Messenger, const VkAllocationCallbacks *Allocator) {
    auto Func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(Instance, "vkDestroyDebugUtilsMessengerEXT");
    if (Func != nullptr) {
        Func(Instance, Messenger, Allocator);
    }
}

FSInt32 VulkanFindGraphicsQueueFamiliy(VkPhysicalDevice GPU) {
    FUInt32 QueueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(GPU, &QueueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> QueueFamilies(QueueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(GPU, &QueueFamilyCount, QueueFamilies.data());

    for (FUInt32 QueueFamilyIndex = 0; QueueFamilyIndex < QueueFamilyCount; QueueFamilyIndex++) {
        if (QueueFamilies[QueueFamilyIndex].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            return QueueFamilyIndex;
        }
    }

    return -1;
}

bool VulkanGetSwapchainSupport(VulkanSwapchainSupport *Out, VkPhysicalDevice Device, VkSurfaceKHR Surface) {
    if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(Device, Surface, &Out->Capabilities) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to get surface capabilities");
        return false;
    }

    FUInt32 FormatCount = 0;
    if (vkGetPhysicalDeviceSurfaceFormatsKHR(Device, Surface, &FormatCount, nullptr) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to get surface format count");
        return false;
    }
    Out->Formats.resize(FormatCount);

    if (vkGetPhysicalDeviceSurfaceFormatsKHR(Device, Surface, &FormatCount, Out->Formats.data()) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to get surface formats");
        return false;
    }

    FUInt32 PresentModeCount = 0;
    if (vkGetPhysicalDeviceSurfacePresentModesKHR(Device, Surface, &PresentModeCount, nullptr) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to get surface present mode count");
        return false;
    }
    Out->PresentModes.resize(PresentModeCount);

    if (vkGetPhysicalDeviceSurfacePresentModesKHR(Device, Surface, &PresentModeCount, Out->PresentModes.data()) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to get surface present modes");
        return false;
    }

    return true;
}

VkAccessFlags VulkanGetAccessMask(ERHIResourceState State) {
    switch (State) {
    case ERHIResourceState::RENDER_TARGET:
        return VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    case ERHIResourceState::PRESENT:
        return 0;
    case ERHIResourceState::SHADER_RESOURCE:
        return VK_ACCESS_SHADER_READ_BIT;
    default:
        return 0;
    }
}

VkImageLayout VulkanGetImageLayout(ERHIResourceState State) {
    switch (State) {
    case ERHIResourceState::RENDER_TARGET:
        return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    case ERHIResourceState::PRESENT:
        return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    case ERHIResourceState::SHADER_RESOURCE:
        return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    default:
        return VK_IMAGE_LAYOUT_UNDEFINED;
    }
}

EVulkanQueue VulkanGetQueue(ERHIResourceState State) {
    switch (State) {
    case ERHIResourceState::RENDER_TARGET:
        return EVulkanQueue::GRAPHICS;
    case ERHIResourceState::PRESENT:
        return EVulkanQueue::PRESENT;
    default:
        return EVulkanQueue::ANY;
    }
}

VkPipelineStageFlags VulkanGetPipelineStageMask(ERHIResourceState State) {
    switch (State) {
    case ERHIResourceState::RENDER_TARGET:
        return VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    case ERHIResourceState::PRESENT:
        return VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    case ERHIResourceState::SHADER_RESOURCE:
        return VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    default:
        return VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    }
}

VkIndexType VulkanGetIndexType(ERHIFormat Format) {
    switch (Format) {
    case ERHIFormat::UNDEFINED:
    default:
        SK_LOG_ERROR("Invalid format for index buffer");
        return (VkIndexType) 0;
    }
}

VkDescriptorType VulkanGetDescriptorType(ERHIDescriptorType Type) {
    switch (Type) {
    case ERHIDescriptorType::UNIFORM_BUFFER:
        return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    case ERHIDescriptorType::TEXTURE:
        return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    default:
        SK_LOG_ERROR("Invalid descriptor type");
        return (VkDescriptorType)0;
    }
}

VkShaderStageFlags VulkanGetShaderStageFlags(std::vector<ERHIShaderType> Types) {
    VkShaderStageFlags Flags = 0;
    for (auto Type : Types) {
        switch (Type) {
        case ERHIShaderType::VERTEX:
            Flags |= VK_SHADER_STAGE_VERTEX_BIT;
            break;
        case ERHIShaderType::FRAGMENT:
            Flags |= VK_SHADER_STAGE_FRAGMENT_BIT;
            break;
        }
    }

    return Flags;
}

VkShaderStageFlagBits VulkanGetShaderStage(ERHIShaderType Type) {
    switch (Type) {
    case ERHIShaderType::VERTEX:
        return VK_SHADER_STAGE_VERTEX_BIT;
    case ERHIShaderType::FRAGMENT:
        return VK_SHADER_STAGE_FRAGMENT_BIT;
    default:
        SK_LOG_ERROR("Invalid shader stage");
        return(VkShaderStageFlagBits)0;
    }
}

VkVertexInputRate VulkanGetVertexInputRate(ERHIVertexInputRate InputRate) {
    switch (InputRate) {
    case ERHIVertexInputRate::PER_VERTEX:
        return VK_VERTEX_INPUT_RATE_VERTEX;
    case ERHIVertexInputRate::PER_INSTANCE:
        return VK_VERTEX_INPUT_RATE_INSTANCE;
    default:
        SK_LOG_ERROR("Invalid vertex input rate");
        return (VkVertexInputRate)0;
    }
}

VkFormat VulkanGetFormat(ERHIFormat Format) {
    switch (Format) {
    case ERHIFormat::B8G8R8A8_SRGB:
        return VK_FORMAT_B8G8R8A8_SRGB;
    default:
        return VK_FORMAT_UNDEFINED;
    }
}

VkFormat VulkanGetDepthFormat(ERHIFormat Format) {
    switch (Format) {
    case ERHIFormat::UNDEFINED:
        return VK_FORMAT_UNDEFINED;
    default:
        return (VkFormat)0;
    }
}

ERHIFormat VulkanGetRHIFormat(VkFormat Format) {
    switch (Format) {
    case VK_FORMAT_B8G8R8A8_SRGB:
        return ERHIFormat::B8G8R8A8_SRGB;
    default:
        SK_LOG_ERROR("Invalid vulkan format");
        return ERHIFormat::UNDEFINED;
    }
}

VkFormat VulkanGetStencilFormat(ERHIFormat Format) {
    switch (Format) {
    case ERHIFormat::UNDEFINED:
        return VK_FORMAT_UNDEFINED;
    default:
        SK_LOG_ERROR("Invalid stencil format");
        return (VkFormat)0;
    }
}

FSInt32 FindPresentQueueIndex(VkSurfaceKHR Surface) {
    FUInt32 QueueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(VulkanContext.GPU, &QueueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> QueueFamilies(QueueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(VulkanContext.GPU, &QueueFamilyCount, QueueFamilies.data());

    for (FUInt32 QueueFamilyIndex = 0; QueueFamilyIndex < QueueFamilyCount; QueueFamilyIndex++) {
        VkBool32 PresentSupport = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(VulkanContext.GPU, QueueFamilyIndex, Surface, &PresentSupport);
        if (PresentSupport) {
            return QueueFamilyIndex;
        }
    }

    return -1;
}

FUInt32 VulkanFindMemoryType(FUInt32 Filter, VkMemoryPropertyFlags Flags) {
    VkPhysicalDeviceMemoryProperties MemProps;
    vkGetPhysicalDeviceMemoryProperties(VulkanContext.GPU, &MemProps);

    for (FUInt32 MemoryTypeIndex = 0; MemoryTypeIndex < MemProps.memoryTypeCount; MemoryTypeIndex++) {
        if ((Filter & (1 << MemoryTypeIndex)) && ((MemProps.memoryTypes[MemoryTypeIndex].propertyFlags & Flags) == Flags)) {
            return MemoryTypeIndex;
        }
    }

    SK_LOG_ERROR("Failed to get memory type");
    return 0;
}
VkCommandBuffer VulkanBeginOneTimeCommandBuffer() {
    VkCommandBufferAllocateInfo AllocInfo = {};
    AllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    AllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    AllocInfo.commandPool = VulkanContext.CommandPool;
    AllocInfo.commandBufferCount = 1;

    VkCommandBuffer CommandBuffer;
    if (vkAllocateCommandBuffers(VulkanContext.Device, &AllocInfo, &CommandBuffer) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to allocate one time command buffer");
        return nullptr;
    }

    VkCommandBufferBeginInfo BeginInfo = {};
    BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    BeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    if (vkBeginCommandBuffer(CommandBuffer, &BeginInfo) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to begin one time command buffer");
        vkFreeCommandBuffers(VulkanContext.Device, VulkanContext.CommandPool, 1, &CommandBuffer);
        return nullptr;
    }

    return CommandBuffer;
}

bool VulkanEndOneTimeCommandBuffer(VkCommandBuffer CommandBuffer) {
    if (CommandBuffer == nullptr) {
        SK_LOG_ERROR("Invalid command buffer provided");
        return false;
    }

    vkEndCommandBuffer(CommandBuffer);

    VkSubmitInfo SubmitInfo = {};
    SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    SubmitInfo.commandBufferCount = 1;
    SubmitInfo.pCommandBuffers = &CommandBuffer;

    if (vkQueueSubmit(VulkanContext.GraphicsQueue, 1, &SubmitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to submit one time command buffer");
        vkFreeCommandBuffers(VulkanContext.Device, VulkanContext.CommandPool, 1, &CommandBuffer);
        return false;
    }
    if (vkQueueWaitIdle(VulkanContext.GraphicsQueue) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to wait for queue");
        vkFreeCommandBuffers(VulkanContext.Device, VulkanContext.CommandPool, 1, &CommandBuffer);
        return false;
    }

    vkFreeCommandBuffers(VulkanContext.Device, VulkanContext.CommandPool, 1, &CommandBuffer);

    return true;
}

bool VulkanCopyBuffer(VkBuffer Src, VkBuffer Dst, FUInt32 Size) {
    VkCommandBuffer CommandBuffer = VulkanBeginOneTimeCommandBuffer();

    if (CommandBuffer == nullptr) {
        SK_LOG_ERROR("Failed to get command buffer for buffer copy");
        return false;
    }

    VkBufferCopy CopyRegion = {};
    CopyRegion.srcOffset = 0;
    CopyRegion.dstOffset = 0;
    CopyRegion.size = Size;

    vkCmdCopyBuffer(CommandBuffer, Src, Dst, 1, &CopyRegion);

    if (!VulkanEndOneTimeCommandBuffer(CommandBuffer)) {
        SK_LOG_ERROR("Failed to submit command buffer for buffer copy");
        return false;
    }

    return true;
}

bool VulkanCreateBuffer(FUInt32 Size, VkBufferUsageFlags Usage, VkMemoryPropertyFlags Properties, VkBuffer *Buffer, VkDeviceMemory *Memory) {
    VkBufferCreateInfo BufferInfo = {};
    BufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    BufferInfo.size = Size;
    BufferInfo.usage = Usage;
    BufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(VulkanContext.Device, &BufferInfo, nullptr, Buffer) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to create buffer");
        return false;
    }

    VkMemoryRequirements MemReqs = {};
    vkGetBufferMemoryRequirements(VulkanContext.Device, *Buffer, &MemReqs);

    VkMemoryAllocateInfo AllocInfo = {};
    AllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    AllocInfo.allocationSize = MemReqs.size;
    AllocInfo.memoryTypeIndex = VulkanFindMemoryType(MemReqs.memoryTypeBits, Properties);

    if (vkAllocateMemory(VulkanContext.Device, &AllocInfo, nullptr, Memory) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to allocate buffer memory");
        return false;
    }

    if (vkBindBufferMemory(VulkanContext.Device, *Buffer, *Memory, 0) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to bind buffer memory");
        return false;
    }

    return true;
}

bool VulkanCreateSwapchainTexture(FVulkanTexture *Out, VkImage Image, VkExtent2D Extent, VkFormat Format) {
    Out->SwapchainTexture = true;
    Out->Extent = Extent;
    Out->Format = Format;
    Out->Image = Image;
    Out->RenderArea = { 1, 0, 0, Extent.width, Extent.height };

    if (!VulkanCreateImageView(&Out->ImageView, VulkanContext.Device, Image, Out->Format)) {
        SK_LOG_ERROR("Failed to create image view");
        return false;
    }

    Out->SubresourceRanges = (VkImageSubresourceRange *)malloc(sizeof(VkImageSubresourceRange));
    Out->SubresourceRangeCount = 1;
    Out->SubresourceRanges[0] = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

    return true;
}

bool VulkanCreateImageView(VkImageView *Out, VkDevice Device, VkImage Image, VkFormat Format) {
    VkImageViewCreateInfo ViewInfo = {};
    ViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    ViewInfo.image = Image;
    ViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    ViewInfo.format = Format;
    ViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    ViewInfo.subresourceRange.baseMipLevel = 0;
    ViewInfo.subresourceRange.levelCount = 1;
    ViewInfo.subresourceRange.baseArrayLayer = 0;
    ViewInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(Device, &ViewInfo, nullptr, Out) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to create image view");
        return false;
    }

    return true;
}

bool VulkanCreateImage(VkDevice Device, VkPhysicalDevice GPU, FUInt32 Width, FUInt32 Height, VkFormat Format, VkImageTiling Tiling, VkImageUsageFlags Flags, VkMemoryPropertyFlags Properties, VkImage *OutImage, VkDeviceMemory *OutImageMemory) {
    VkImageCreateInfo ImageInfo = {};
    ImageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    ImageInfo.imageType = VK_IMAGE_TYPE_2D;
    ImageInfo.extent.width = Width;
    ImageInfo.extent.height = Height;
    ImageInfo.extent.depth = 1;
    ImageInfo.mipLevels = 1;
    ImageInfo.arrayLayers = 1;
    ImageInfo.format = Format;
    ImageInfo.tiling = Tiling;
    ImageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    ImageInfo.usage = Flags;
    ImageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    ImageInfo.samples = VK_SAMPLE_COUNT_1_BIT;

    if (vkCreateImage(Device, &ImageInfo, nullptr, OutImage) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to create image");
        return false;
    }

    VkMemoryRequirements MemReqs = {};
    vkGetImageMemoryRequirements(Device, *OutImage, &MemReqs);

    VkMemoryAllocateInfo AllocInfo = {};
    AllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    AllocInfo.allocationSize = MemReqs.size;
    AllocInfo.memoryTypeIndex = VulkanFindMemoryType(MemReqs.memoryTypeBits, Properties);

    if (vkAllocateMemory(Device, &AllocInfo, nullptr, OutImageMemory) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to allocate image memory");
        return false;
    }

    vkBindImageMemory(Device, *OutImage, *OutImageMemory, 0);

    return true;
}

bool VulkanCreateTextureSampler(VkSampler *Out, VkDevice Device, VkPhysicalDevice GPU) {
    VkSamplerCreateInfo SamplerInfo = {};
    SamplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    SamplerInfo.magFilter = VK_FILTER_LINEAR;
    SamplerInfo.minFilter = VK_FILTER_LINEAR;
    SamplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    SamplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    SamplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    SamplerInfo.anisotropyEnable = VK_TRUE;

    VkPhysicalDeviceProperties Properties = { 0 };
    vkGetPhysicalDeviceProperties(GPU, &Properties);
    SamplerInfo.maxAnisotropy = Properties.limits.maxSamplerAnisotropy;

    SamplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    SamplerInfo.unnormalizedCoordinates = VK_FALSE;
    SamplerInfo.compareEnable = VK_FALSE;
    SamplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    SamplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    SamplerInfo.mipLodBias = 0.0f;
    SamplerInfo.minLod = 0.0f;
    SamplerInfo.maxLod = 0.0f;

    if (vkCreateSampler(Device, &SamplerInfo, nullptr, Out) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to create image sampler");
        return false;
    }

    return true;
}

void VulkanTransitionBarrier(FVulkanCommandList *List, FRHITransitionBarrier *Barrier) {
    switch (RHIGetTransitionType(Barrier->StateBefore, Barrier->StateAfter)) {
    case ERHITransitionType::IMAGE: {
        auto Texture = (FVulkanTexture *)Barrier->Resource;

        VkImageMemoryBarrier ImageBarrier = {};
        ImageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        ImageBarrier.srcAccessMask = VulkanGetAccessMask(Barrier->StateBefore);
        ImageBarrier.dstAccessMask = VulkanGetAccessMask(Barrier->StateAfter);
        ImageBarrier.oldLayout = VulkanGetImageLayout(Barrier->StateBefore);
        ImageBarrier.newLayout = VulkanGetImageLayout(Barrier->StateAfter);
        EVulkanQueue SrcQueue = VulkanGetQueue(Barrier->StateAfter);
        if (SrcQueue == EVulkanQueue::ANY) {
            ImageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        } else if (SrcQueue == EVulkanQueue::GRAPHICS) {
            ImageBarrier.srcQueueFamilyIndex = VulkanContext.GraphicsQueueIndex;
        } else if (SrcQueue == EVulkanQueue::PRESENT) {
            ImageBarrier.srcQueueFamilyIndex = VulkanContext.ActiveSwapchain->PresentQueueIndex;
        } else {
            SK_LOG_ERROR("Unsupported source queue");
            return;
        }
        EVulkanQueue DstQueue = VulkanGetQueue(Barrier->StateAfter);
        if (DstQueue == EVulkanQueue::ANY) {
            ImageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        } else if (DstQueue == EVulkanQueue::GRAPHICS) {
            ImageBarrier.dstQueueFamilyIndex = VulkanContext.GraphicsQueueIndex;
        } else if (DstQueue == EVulkanQueue::PRESENT) {
            ImageBarrier.dstQueueFamilyIndex = VulkanContext.ActiveSwapchain->PresentQueueIndex;
        } else {
            SK_LOG_ERROR("Unsupported destination queue");
            return;
        }

        ImageBarrier.image = Texture->Image;
        ImageBarrier.subresourceRange = Texture->SubresourceRanges[Barrier->Subresource];

        // TODO: Batch pipeline barrier calls
        vkCmdPipelineBarrier(List->Buffers[VulkanContext.CurrentFrame], VulkanGetPipelineStageMask(Barrier->StateBefore), VulkanGetPipelineStageMask(Barrier->StateAfter), 0, 0, nullptr, 0, nullptr, 1, &ImageBarrier);
        break;
    }
    default:
        SK_LOG_WARN("Unsupported resource transition type");
        return;
    }
}


// Rhi funcions

FRHI VulkanCreateRHI() {
    return {
        VulkanInit,
        VulkanShutdown,
        VulkanImGuiNewFrame,
        VulkanShutdownImGui,
        VulkanInitImGui,
        VulkanWaitForGPUIdle,
        VulkanPrepareFrame,
        VulkanPresentFrame,
        VulkanSubmit,
        VulkanAddTextureToImGuiWindow,
        VulkanCreateTexture,
        VulkanDestroyTexture,
        VulkanCreateSwapchain,
        VulkanDestroySwapchain,
        VulkanCreateCommandList,
        VulkanDestroyCommandList,
        VulkanCreateShader,
        VulkanDestroyShader,
        VulkanCreateVertexBuffer,
        VulkanCreateIndexBuffer,
        VulkanDestroyBuffer,
        VulkanCreatePipelineLayout,
        VulkanDestroyPipelineLayout,
        VulkanCreateDescriptorSetLayout,
        VulkanDestroyDescriptorSetLayout,
        VulkanCreateGraphicsPipeline,
        VulkanDestroyPipeline,
        VulkanSetBufferData,
        VulkanMapBufferData,
        VulkanSendBufferToGPU,
        VulkanGetCurrentSwapchainTexture,
        VulkanGetTextureFormat,
        VulkanGetTextureRenderArea,
        VulkanBeginCommandList,
        VulkanEndCommandList,
        VulkanCmdRenderImGuiDrawData,
        VulkanCmdResourceBarrier,
        VulkanCmdSetRenderTarget,
        VulkanCmdUnsetRenderTarget,
        VulkanCmdBindVertexBuffer,
        VulkanCmdBindIndexBuffer,
        VulkanCmdDrawIndexed,
        VulkanCmdDrawInstanced,
        VulkanCmdSetViewport,
        VulkanCmdSetScissor,
        VulkanCmdBindPipeline,
        VulkanNotifySwapchainOfResize,
    };
}

bool VulkanInit() {
    FBool ExtensionsSupported = true;
    FUInt32 LayerCount = 0;
    vkEnumerateInstanceLayerProperties(&LayerCount, nullptr);

    std::vector<VkLayerProperties> AvailableLayers(LayerCount);
    vkEnumerateInstanceLayerProperties(&LayerCount, AvailableLayers.data());

    for (auto &ValidationLayer : VALIDATION_LAYERS) {
        FBool LayerFound = false;

        for (auto &AvailableLayer : AvailableLayers) {
            if (strcmp(ValidationLayer, AvailableLayer.layerName) != 0) {
                LayerFound = true;
            }
        }

        if (!LayerFound) {
            ExtensionsSupported = false;
            break;
        }
    }

    if (ENABLE_VALIDATION_LAYERS && !ExtensionsSupported) {
        SK_LOG_ERROR("Validation layers not supported");
        return false;
    }

    VkApplicationInfo AppInfo = {};
    AppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    AppInfo.pApplicationName = "App Name"; // TODO:
    AppInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    AppInfo.pEngineName = "StakEngine";
    AppInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    AppInfo.apiVersion = VK_API_VERSION_1_3;

    VkInstanceCreateInfo InstanceInfo = {};
    InstanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    InstanceInfo.pApplicationInfo = &AppInfo;

    VkDebugUtilsMessengerCreateInfoEXT DebugMessengerInfo = {};

    if (ENABLE_VALIDATION_LAYERS) {
        InstanceInfo.enabledLayerCount = (FUInt32)VALIDATION_LAYERS.size();
        InstanceInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();

        DebugMessengerInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        DebugMessengerInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        DebugMessengerInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        DebugMessengerInfo.pfnUserCallback = VulkanMessenger;

        InstanceInfo.pNext = &DebugMessengerInfo;
    }

#ifdef SK_WINDOWS
    std::vector<const char *> Extensions = {
        "VK_KHR_surface",
        "VK_KHR_win32_surface",
    };
#else
    std::vector<const char *> Extensions;
#endif

    if (ENABLE_VALIDATION_LAYERS) {
        Extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    InstanceInfo.enabledExtensionCount = (FUInt32)Extensions.size();
    InstanceInfo.ppEnabledExtensionNames = Extensions.data();

    CHECK_VK_ERR(vkCreateInstance(&InstanceInfo, nullptr, &VulkanContext.Instance), "Failed to create vulkan instance");
    CHECK_VK_ERR(VulkanCreateDebugMessenger(VulkanContext.Instance, &DebugMessengerInfo, nullptr, &VulkanContext.DebugMessenger), "Failed to create vulkan debug messenger");

    FUInt32 DeviceCount = 0;
    vkEnumeratePhysicalDevices(VulkanContext.Instance, &DeviceCount, nullptr);
    std::vector<VkPhysicalDevice> Devices(DeviceCount);
    vkEnumeratePhysicalDevices(VulkanContext.Instance, &DeviceCount, Devices.data());

    FBool DeviceFound = false;
    for (FUInt32 DeviceIndex = 0; DeviceIndex < DeviceCount; DeviceIndex++) {
        FBool IsDeviceSuitable = false;
        VulkanContext.GraphicsQueueIndex = VulkanFindGraphicsQueueFamiliy(Devices[DeviceIndex]);

        FUInt32 DeviceExtensionCount = 0;
        vkEnumerateDeviceExtensionProperties(Devices[DeviceIndex], nullptr, &DeviceExtensionCount, nullptr);
        std::vector<VkExtensionProperties> AvailableExtensions(DeviceExtensionCount);

        vkEnumerateDeviceExtensionProperties(Devices[DeviceIndex], nullptr, &DeviceExtensionCount, AvailableExtensions.data());

        FBool ExtensionsSupported = false;

        for (FUInt32 DeviceExtensionIndex = 0; DeviceExtensionIndex < DeviceExtensionCount; DeviceExtensionIndex++) {
            if (std::strcmp(AvailableExtensions[DeviceExtensionIndex].extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME) != 0) {
                ExtensionsSupported = true;
                break;
            }
        }

        VkPhysicalDeviceFeatures SupportedFeatures;
        vkGetPhysicalDeviceFeatures(Devices[DeviceIndex], &SupportedFeatures);

        IsDeviceSuitable = ExtensionsSupported && VulkanContext.GraphicsQueueIndex >= 0 && SupportedFeatures.samplerAnisotropy;

        if (IsDeviceSuitable) {
            VulkanContext.GPU = Devices[DeviceIndex];
            DeviceFound = true;
            break;
        }
    }

    if (!DeviceFound) {
        SK_LOG_ERROR("Failed to find suitable physical device");
        return false;
    }

    FFloat QueuePriority = 1.0f;
    VkDeviceQueueCreateInfo QueueInfo = {};
    QueueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    QueueInfo.queueFamilyIndex = VulkanContext.GraphicsQueueIndex;
    QueueInfo.queueCount = 1;
    QueueInfo.pQueuePriorities = &QueuePriority;

    VkPhysicalDeviceFeatures DeviceFeatures = {};
    DeviceFeatures.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo DeviceInfo = {};
    DeviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    VkPhysicalDeviceDynamicRenderingFeatures DynamicRenderingFeatures = {};
    DynamicRenderingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES;
    DynamicRenderingFeatures.dynamicRendering = VK_TRUE;
    DeviceInfo.pNext = &DynamicRenderingFeatures;

    VkPhysicalDeviceMultiviewFeatures MultiviewFeatures = {};
    MultiviewFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES;
    MultiviewFeatures.multiview = VK_TRUE;

    DynamicRenderingFeatures.pNext = &MultiviewFeatures;

    VkPhysicalDeviceSynchronization2Features Sync2Features = {};
    Sync2Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES;
    Sync2Features.synchronization2 = VK_TRUE;

    MultiviewFeatures.pNext = &Sync2Features;

    DeviceInfo.queueCreateInfoCount = 1;
    DeviceInfo.pQueueCreateInfos = &QueueInfo;
    DeviceInfo.pEnabledFeatures = &DeviceFeatures;
    DeviceInfo.enabledExtensionCount = (FUInt32)DEVICE_EXTENSIONS.size();
    DeviceInfo.ppEnabledExtensionNames = DEVICE_EXTENSIONS.data();

    if (ENABLE_VALIDATION_LAYERS) {
        DeviceInfo.enabledLayerCount = (FUInt32)VALIDATION_LAYERS.size();
        DeviceInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();
    }

    if (vkCreateDevice(VulkanContext.GPU, &DeviceInfo, nullptr, &VulkanContext.Device) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to create vulkan device");
        return false;
    }

    vkGetDeviceQueue(VulkanContext.Device, VulkanContext.GraphicsQueueIndex, 0, &VulkanContext.GraphicsQueue);

    VkFenceCreateInfo FenceInfo = {};
    FenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    FenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (auto &Fence : VulkanContext.InFlightFences) {
        if (vkCreateFence(VulkanContext.Device, &FenceInfo, nullptr, &Fence) != VK_SUCCESS) {
            SK_LOG_ERROR("Failed to create vulkan fence");
            return false;
        }
    }

    VkCommandPoolCreateInfo CommandPoolInfo = {};

    CommandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    CommandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    CommandPoolInfo.queueFamilyIndex = VulkanContext.GraphicsQueueIndex;

    if (vkCreateCommandPool(VulkanContext.Device, &CommandPoolInfo, nullptr, &VulkanContext.CommandPool) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to create vulkan command pool");
        return false;
    }

    return true;
}

void VulkanShutdown() {
    for (auto &Fence : VulkanContext.InFlightFences) {
        vkDestroyFence(VulkanContext.Device, Fence, nullptr);
    }

    vkDestroyCommandPool(VulkanContext.Device, VulkanContext.CommandPool, nullptr);

    vkDestroyDevice(VulkanContext.Device, nullptr);

    VulkanDestroyDebugMessenger(VulkanContext.Instance, VulkanContext.DebugMessenger, nullptr);

    vkDestroyInstance(VulkanContext.Instance, nullptr);
}

bool VulkanRecreateSwapchain(FVulkanSwapchain *Swapchain) {
    Swapchain->ImageIndex = 0;

#ifdef SK_GLFW
    FSInt32 Width = 0;
    FSInt32 Height = 0;
    auto Window = (GLFWwindow *)Swapchain->WindowHandle;

    glfwGetFramebufferSize(Window, &Width, &Height);

    while (Width == 0 || Height == 0) {
        glfwPollEvents();
        glfwGetFramebufferSize(Window, &Width, &Height);
    }
#endif

    if (!GRHI.WaitForGPUIdle()) {
        SK_LOG_ERROR("Failed to wait for gpu before swapchain recreate");
        return false;
    }

    for (FUInt32 ImageIndex = 0; ImageIndex < Swapchain->ImageCount; ImageIndex++)
        vkDestroyImageView(VulkanContext.Device, Swapchain->Images[ImageIndex].ImageView, nullptr);

    vkDestroySwapchainKHR(VulkanContext.Device, Swapchain->Swapchain, nullptr);

    VulkanSwapchainSupport Support;
    if (!VulkanGetSwapchainSupport(&Support, VulkanContext.GPU, Swapchain->Surface)) {
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
        glfwGetFramebufferSize(Window, &Width, &Height);
#endif

        VkExtent2D Extent = {
            (FUInt32)Width,
            (FUInt32)Height
        };

        Extent.width = std::clamp(Extent.width, Support.Capabilities.minImageExtent.width, Support.Capabilities.maxImageExtent.width);
        Extent.height = std::clamp(Extent.height, Support.Capabilities.minImageExtent.height, Support.Capabilities.maxImageExtent.height);

        SwapchainExtent = Extent;
    }

    FUInt32 ImageCount = Support.Capabilities.minImageCount + 1;

    if (Support.Capabilities.maxImageCount > 0 && ImageCount > Support.Capabilities.maxImageCount) {
        ImageCount = Support.Capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR SwapchainInfo = {};
    SwapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    SwapchainInfo.surface = Swapchain->Surface;
    SwapchainInfo.minImageCount = Swapchain->ImageCount;
    SwapchainInfo.imageFormat = Format.format;
    SwapchainInfo.imageColorSpace = Format.colorSpace;
    SwapchainInfo.imageExtent = SwapchainExtent;
    SwapchainInfo.imageArrayLayers = 1;
    SwapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    Swapchain->PresentQueueIndex = FindPresentQueueIndex(Swapchain->Surface);

    assert(VulkanContext.GraphicsQueueIndex == Swapchain->PresentQueueIndex);

    SwapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    SwapchainInfo.preTransform = Support.Capabilities.currentTransform;
    SwapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    SwapchainInfo.presentMode = PresentMode;
    SwapchainInfo.clipped = VK_TRUE;
    SwapchainInfo.oldSwapchain = VK_NULL_HANDLE;
    if (vkCreateSwapchainKHR(VulkanContext.Device, &SwapchainInfo, nullptr, &Swapchain->Swapchain) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to create swapchian");
        return false;
    }

    VkImage *Images = (VkImage *)malloc(sizeof(VkImage) * ImageCount);
    vkGetSwapchainImagesKHR(VulkanContext.Device, Swapchain->Swapchain, &ImageCount, Images);

    if (Swapchain->ImageCount != ImageCount)
        Swapchain->Images = (FVulkanTexture *)realloc(Swapchain->Images, sizeof(FVulkanTexture) * ImageCount);

    for (FUInt32 BackbufferIndex = 0; BackbufferIndex < Swapchain->ImageCount; BackbufferIndex++) {
        if (!VulkanCreateSwapchainTexture(&Swapchain->Images[BackbufferIndex], Images[BackbufferIndex], SwapchainExtent, Format.format)) {
            SK_LOG_ERROR("Failed to create backbuffer");
            return false;
        }
    }

    Swapchain->FramebufferResized = false;
    Swapchain->ImageCount = ImageCount;
    Swapchain->MinImageCount = ImageCount;

    return true;
}

void VulkanImGuiNewFrame() {
    ImGui_ImplVulkan_NewFrame();
}

void VulkanShutdownImGui() {
    ImGui_ImplVulkan_Shutdown();
    vkDestroyDescriptorPool(VulkanContext.Device, VulkanContext.ImGuiPool, nullptr);
}

void VulkanInitImGui(FRHIResourceHandle Swapchain) {
    auto VulkanSwapchain = (FVulkanSwapchain *)Swapchain;
    VkFormat Formats[] = { VulkanSwapchain->Format };

    VkPipelineRenderingCreateInfo PipelineInfo = {};
    PipelineInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    PipelineInfo.viewMask = 0x01;
    PipelineInfo.colorAttachmentCount = 1;
    PipelineInfo.pColorAttachmentFormats = Formats;
    PipelineInfo.depthAttachmentFormat = VK_FORMAT_UNDEFINED;
    PipelineInfo.stencilAttachmentFormat = VK_FORMAT_UNDEFINED;

    ImGui_ImplVulkan_InitInfo InitInfo = {};
    InitInfo.ApiVersion = VK_API_VERSION_1_3;
    InitInfo.Instance = VulkanContext.Instance;
    InitInfo.PhysicalDevice = VulkanContext.GPU;
    InitInfo.Device = VulkanContext.Device;
    InitInfo.QueueFamily = VulkanContext.GraphicsQueueIndex;
    InitInfo.Queue = VulkanContext.GraphicsQueue;
    InitInfo.MinImageCount = VulkanSwapchain->MinImageCount;
    InitInfo.ImageCount = VulkanSwapchain->ImageCount;
    InitInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    InitInfo.DescriptorPoolSize = 1000;
    InitInfo.UseDynamicRendering = true;
    InitInfo.PipelineRenderingCreateInfo = PipelineInfo;
    InitInfo.Allocator = nullptr;
    InitInfo.CheckVkResultFn = [](VkResult Err) {
        CHECK_VK_ERR(Err, "ImGui Vulkan Error");
    };
    InitInfo.MinAllocationSize = 1024 * 1024;

    ImGui_ImplVulkan_Init(&InitInfo);
}

bool VulkanWaitForGPUIdle() {
    if (vkDeviceWaitIdle(VulkanContext.Device) != VK_SUCCESS) return false;
    return true;
}

bool VulkanPrepareFrame(FRHIResourceHandle Swapchain) {
    if (VulkanContext.ActiveSwapchain) {
        SK_LOG_ERROR("Swapchain already set");
        return false;
    }

    auto VulkanSwapchain = (FVulkanSwapchain *)Swapchain;
    if (vkAcquireNextImageKHR(VulkanContext.Device, VulkanSwapchain->Swapchain, UINT64_MAX, VulkanSwapchain->ImageAvailableSemaphores[VulkanContext.CurrentFrame], nullptr, &VulkanSwapchain->ImageIndex) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to get next swapchain image");
        return false;
    }

    VulkanContext.ActiveSwapchain = VulkanSwapchain;

    return true;
}

bool VulkanPresentFrame() {
    if (VulkanContext.ActiveSwapchain == nullptr) {
        SK_LOG_WARN("No swapchain set for rendering");
        return false;
    }

    VkPresentInfoKHR PresentInfo = {};
    PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    PresentInfo.swapchainCount = 1;
    PresentInfo.pSwapchains = &VulkanContext.ActiveSwapchain->Swapchain;
    PresentInfo.waitSemaphoreCount = 1;
    PresentInfo.pWaitSemaphores = &VulkanContext.ActiveSwapchain->RenderFinishedSemaphores[VulkanContext.CurrentFrame];
    PresentInfo.pImageIndices = &VulkanContext.ActiveSwapchain->ImageIndex;
    VkResult Err = vkQueuePresentKHR(VulkanContext.ActiveSwapchain->PresentQueue, &PresentInfo);

    if (Err == VK_ERROR_OUT_OF_DATE_KHR || Err == VK_SUBOPTIMAL_KHR || VulkanContext.ActiveSwapchain->FramebufferResized) {
        if (!VulkanRecreateSwapchain(VulkanContext.ActiveSwapchain)) {
            SK_LOG_ERROR("Failed to recreate swapchian");
            return false;
        }
    } else if (Err != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to present");
        return false;
    }

    VulkanContext.CurrentFrame = (VulkanContext.CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

    if (vkWaitForFences(VulkanContext.Device, 1, &VulkanContext.InFlightFences[VulkanContext.CurrentFrame], VK_TRUE, UINT64_MAX) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to wait for fences");
        return false;
    }

    VulkanContext.ActiveSwapchain = nullptr;

    return true;
}

bool VulkanSubmit(FRHIResourceHandle List) {
    if (VulkanContext.ActiveSwapchain == nullptr) {
        SK_LOG_WARN("No swapchain set for rendering");
        return false;
    }

    auto CommandList = (FVulkanCommandList *)List;
    VkCommandBuffer CommandBuffer = CommandList->Buffers[VulkanContext.CurrentFrame];

    VkSubmitInfo SubmitInfo = {};
    SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    VkPipelineStageFlags WaitStages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
    };
    SubmitInfo.waitSemaphoreCount = 1;
    SubmitInfo.pWaitSemaphores = &VulkanContext.ActiveSwapchain->ImageAvailableSemaphores[VulkanContext.CurrentFrame];
    SubmitInfo.pWaitDstStageMask = WaitStages;
    SubmitInfo.signalSemaphoreCount = 1;
    SubmitInfo.pSignalSemaphores = &VulkanContext.ActiveSwapchain->RenderFinishedSemaphores[VulkanContext.CurrentFrame];
    SubmitInfo.commandBufferCount = 1;
    SubmitInfo.pCommandBuffers = &CommandBuffer;

    if (vkResetFences(VulkanContext.Device, 1, &VulkanContext.InFlightFences[VulkanContext.CurrentFrame]) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to reset fencecs");
        return false;
    }

    if (vkQueueSubmit(VulkanContext.GraphicsQueue, 1, &SubmitInfo, VulkanContext.InFlightFences[VulkanContext.CurrentFrame]) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to submit to graphics queue");
        return false;
    }

    return true;
}

void VulkanAddTextureToImGuiWindow(FRHIResourceHandle Texture) {
    auto VulkanTexture = (FVulkanTexture *)Texture;

    ImVec2 ViewportPanelSize = ImGui::GetContentRegionAvail();

    // Keep aspect ratio
    FFloat Aspect = (FFloat)VulkanTexture->Extent.width / (FFloat)VulkanTexture->Extent.height;
    ImVec2 ImageSize;
    if (ViewportPanelSize.x / Aspect <= ViewportPanelSize.y) {
        ImageSize.x = ViewportPanelSize.x;
        ImageSize.y = ViewportPanelSize.x / Aspect;
    } else {
        ImageSize.y = ViewportPanelSize.y;
        ImageSize.x = ViewportPanelSize.y * Aspect;
    }

    // Center Image
    ImVec2 Offset = { (ViewportPanelSize.x - ImageSize.x) * 0.5f, (ViewportPanelSize.y - ImageSize.y) * 0.5f };
    Offset.x = (Offset.x > 0) ? Offset.x : 0;
    Offset.y = (Offset.y > 0) ? Offset.y : 0;
    ImVec2 CursorPos = ImGui::GetCursorPos();
    CursorPos.x += Offset.x;
    CursorPos.y += Offset.y;
    ImGui::SetCursorPos(CursorPos);

    ImGui::Image((FUInt64)VulkanTexture->ImGuiDescriptorSet, ImageSize);
}

FRHIResourceHandle VulkanCreateTexture(FRHITextureDescription *Description) {
    auto Texture = (FVulkanTexture *)malloc(sizeof(FVulkanTexture));

    Texture->SwapchainTexture = false;
    Texture->Format = VulkanGetFormat(Description->Format);
    Texture->Extent = { Description->Width, Description->Height };
    Texture->RenderArea = { 1, 0, 0, Description->Width, Description->Height };

    Texture->IsShaderResource = Description->UseAsShaderResource;
    Texture->IsImGuiCompatible = Description->UseForImGui;

    if (!VulkanCreateImage(VulkanContext.Device, VulkanContext.GPU, Texture->Extent.width, Texture->Extent.height, Texture->Format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &Texture->Image, &Texture->ImageMemory)) {
        SK_LOG_ERROR("Failed to create offscreen render target image");
        return 0;
    }

    if (!VulkanCreateImageView(&Texture->ImageView, VulkanContext.Device, Texture->Image, Texture->Format)) {
        SK_LOG_ERROR("Failed to create image view for offscreen buffer");
        return 0;
    }

    if (Texture->IsShaderResource) {
        if (!VulkanCreateTextureSampler(&Texture->Sampler, VulkanContext.Device, VulkanContext.GPU)) {
            SK_LOG_ERROR("Failed to create sampler for offscreen backbuffer");
            return 0;
        }
    } else {
        Texture->Sampler = nullptr;
    }

    if (Texture->IsImGuiCompatible) {
        Texture->ImGuiDescriptorSet = ImGui_ImplVulkan_AddTexture(Texture->Sampler, Texture->ImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        if (!Texture->ImGuiDescriptorSet) {
            SK_LOG_ERROR("Failed to create imgui descriptor set");
            return 0;
        }
    } else {
        Texture->ImGuiDescriptorSet = nullptr;
    }

    Texture->SubresourceRanges = (VkImageSubresourceRange *)malloc(sizeof(VkImageSubresourceRange));
    Texture->SubresourceRangeCount = 1;
    Texture->SubresourceRanges[0] = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

    return (FRHIResourceHandle)Texture;
}

void VulkanDestroyTexture(FRHIResourceHandle *Resource) {
    auto Texture = (FVulkanTexture *)*Resource;

    if (!Texture->SwapchainTexture) {
        if (Texture->IsImGuiCompatible)
            ImGui_ImplVulkan_RemoveTexture(Texture->ImGuiDescriptorSet);
    }

    vkDestroyImageView(VulkanContext.Device, Texture->ImageView, nullptr);

    if (!Texture->SwapchainTexture) {
        vkFreeMemory(VulkanContext.Device, Texture->ImageMemory, nullptr);
        vkDestroyImage(VulkanContext.Device, Texture->Image, nullptr);
        if (Texture->IsShaderResource)
            vkDestroySampler(VulkanContext.Device, Texture->Sampler, nullptr);
    }

    free(Texture);

    *Resource = 0;
}

FRHIResourceHandle VulkanCreateSwapchain(FWindow *Window) {
    auto Swapchain = (FVulkanSwapchain *)malloc(sizeof(FVulkanSwapchain));

    Swapchain->WindowHandle = Window->PlatformHandle;

#ifdef SK_GLFW
    if (glfwCreateWindowSurface(VulkanContext.Instance, (GLFWwindow *)Window->PlatformHandle, nullptr, &Swapchain->Surface) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to create window surface");
        return 0;
    }
#endif

    VkSemaphoreCreateInfo SemaphoreInfo = {};
    SemaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    for (FUInt32 Index = 0; Index < MAX_FRAMES_IN_FLIGHT; Index++) {
        if (vkCreateSemaphore(VulkanContext.Device, &SemaphoreInfo, nullptr, &Swapchain->ImageAvailableSemaphores[Index]) != VK_SUCCESS) {
            SK_LOG_ERROR("Failed to create vulkan semaphore");
            return 0;
        }
        if (vkCreateSemaphore(VulkanContext.Device, &SemaphoreInfo, nullptr, &Swapchain->RenderFinishedSemaphores[Index]) != VK_SUCCESS) {
            SK_LOG_ERROR("Failed to create vulkan semaphore");
            return 0;
        }
    }

    VulkanSwapchainSupport Support;
    if (!VulkanGetSwapchainSupport(&Support, VulkanContext.GPU, Swapchain->Surface)) {
        SK_LOG_ERROR("Failed to get swapchain support");
        return 0;
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
        glfwGetFramebufferSize((GLFWwindow *)Window, &Width, &Height);
#endif

        VkExtent2D Extent = {
            static_cast<FUInt32>(Width),
            static_cast<FUInt32>(Height)
        };

        Extent.width = std::clamp(Extent.width, Support.Capabilities.minImageExtent.width, Support.Capabilities.maxImageExtent.width);
        Extent.height = std::clamp(Extent.height, Support.Capabilities.minImageExtent.height, Support.Capabilities.maxImageExtent.height);

        SwapchainExtent = Extent;
    }

    Swapchain->ImageCount = Support.Capabilities.minImageCount + 1;

    if (Support.Capabilities.maxImageCount > 0 && Swapchain->ImageCount > Support.Capabilities.maxImageCount) {
        Swapchain->ImageCount = Support.Capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR SwapchainInfo = {};
    SwapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    SwapchainInfo.surface = Swapchain->Surface;
    SwapchainInfo.minImageCount = Swapchain->ImageCount;
    SwapchainInfo.imageFormat = Format.format;
    SwapchainInfo.imageColorSpace = Format.colorSpace;
    SwapchainInfo.imageExtent = SwapchainExtent;
    SwapchainInfo.imageArrayLayers = 1;
    SwapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    Swapchain->MinImageCount = Swapchain->ImageCount;

    Swapchain->PresentQueueIndex = FindPresentQueueIndex(Swapchain->Surface);

    assert(VulkanContext.GraphicsQueueIndex == Swapchain->PresentQueueIndex);

    SwapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    SwapchainInfo.preTransform = Support.Capabilities.currentTransform;
    SwapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    SwapchainInfo.presentMode = PresentMode;
    SwapchainInfo.clipped = VK_TRUE;
    SwapchainInfo.oldSwapchain = VK_NULL_HANDLE;
    if (vkCreateSwapchainKHR(VulkanContext.Device, &SwapchainInfo, nullptr, &Swapchain->Swapchain) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to create swapchian");
        return 0;
    }

    std::vector<VkImage> SwapchainImages = {};
    SwapchainImages.resize(Swapchain->ImageCount);

    vkGetSwapchainImagesKHR(VulkanContext.Device, Swapchain->Swapchain, &Swapchain->ImageCount, SwapchainImages.data());

    Swapchain->Images = (FVulkanTexture *)malloc(Swapchain->ImageCount * sizeof(FVulkanTexture));
    for (FUInt32 Index = 0; Index < Swapchain->ImageCount; Index++) {
        if (!VulkanCreateSwapchainTexture(&Swapchain->Images[Index], SwapchainImages[Index], SwapchainExtent, Format.format)) {
            SK_LOG_ERROR("Failed to create swapchain texture");
            return 0;
        }
    }

    vkGetDeviceQueue(VulkanContext.Device, Swapchain->PresentQueueIndex, 0, &Swapchain->PresentQueue);

    Swapchain->ImageIndex = 0;
    Swapchain->Format = Format.format;

    return (FRHIResourceHandle)Swapchain;
}

void VulkanDestroySwapchain(FRHIResourceHandle *Resource) {
    auto Swapchain = (FVulkanSwapchain *)*Resource;
    for (FUInt32 Index = 0; Index < Swapchain->ImageCount; Index++)
        vkDestroyImageView(VulkanContext.Device, Swapchain->Images[Index].ImageView, nullptr);

    vkDestroySwapchainKHR(VulkanContext.Device, Swapchain->Swapchain, nullptr);

    vkDestroySurfaceKHR(VulkanContext.Instance, Swapchain->Surface, nullptr);

    for (FUInt32 Index = 0; Index < MAX_FRAMES_IN_FLIGHT; Index++) {
        vkDestroySemaphore(VulkanContext.Device, Swapchain->ImageAvailableSemaphores[Index], nullptr);
        vkDestroySemaphore(VulkanContext.Device, Swapchain->RenderFinishedSemaphores[Index], nullptr);
    }

    free(Swapchain);
    *Resource = 0;
}

FRHIResourceHandle VulkanCreateCommandList(FRHICommandListDescription *Description) {
    auto CommandList = (FVulkanCommandList *)malloc(sizeof(FVulkanCommandList));

    VkCommandBufferAllocateInfo Info = {};
    Info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    Info.commandBufferCount = MAX_FRAMES_IN_FLIGHT;
    Info.commandPool = VulkanContext.CommandPool;
    Info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    if (Description->IsSecondary) {
        Info.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
    }

    if (vkAllocateCommandBuffers(VulkanContext.Device, &Info, CommandList->Buffers) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to allocate command buffers");
        return 0;
    }

    return (FRHIResourceHandle)CommandList;
}

void VulkanDestroyCommandList(FRHIResourceHandle *Resource) {
    auto CommandList = (FVulkanCommandList *)*Resource;
    vkFreeCommandBuffers(VulkanContext.Device, VulkanContext.CommandPool, MAX_FRAMES_IN_FLIGHT, CommandList->Buffers);
    free(CommandList);
    *Resource = 0;
}

FRHIResourceHandle VulkanCreateShader(FRHIShaderDescription *Description) {
    auto Shader = (FVulkanShader *)malloc(sizeof(FVulkanShader));

    Shader->Stage = VulkanGetShaderStage(Description->Type);

    std::string Path = "Assets/Shaders/";
    Path.append(Description->Name);
    Path.append(".spv");
    std::ifstream File(Path, std::ios::ate | std::ios::binary);

    if (!File.is_open()) {
        SK_LOG_ERROR("Failed to open shader file");
        return 0;
    }

    FUInt32 FileSize = static_cast<FUInt32>(File.tellg());
    std::vector<char> ShaderCode(FileSize);
    File.seekg(0);
    File.read(ShaderCode.data(), FileSize);
    File.close();

    VkShaderModuleCreateInfo Info = {};
    Info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    Info.codeSize = static_cast<FUInt32>(ShaderCode.size());
    Info.pCode = reinterpret_cast<const FUInt32 *>(ShaderCode.data());

    if (vkCreateShaderModule(VulkanContext.Device, &Info, nullptr, &Shader->Shader) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to create shader module");
        return 0;
    }

    return (FRHIResourceHandle)Shader;
}

void VulkanDestroyShader(FRHIResourceHandle *Resource) {
    auto Shader = (FVulkanShader *)*Resource;
    vkDestroyShaderModule(VulkanContext.Device, Shader->Shader, nullptr);
    free(Shader);
    *Resource = 0;
}

FRHIResourceHandle VulkanCreateVertexBuffer(FRHIVertexBufferDescription *Description) {
    auto Buffer = (FVulkanBuffer *)malloc(sizeof(FVulkanBuffer));

    Buffer->UseStagingBuffer = Description->UseStagingBuffer;
    Buffer->ElementCount = Description->ElementCount;
    Buffer->Size = Description->ElementCount * Description->Layout.Stride;
    Buffer->Type = ERHIBufferType::VERTEX;
    Buffer->Layout = Description->Layout;

    if (Buffer->UseStagingBuffer) {
        if (!VulkanCreateBuffer(Buffer->Size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &Buffer->StagingBuffer, &Buffer->StagingBufferMemory)) {
            SK_LOG_ERROR("Failed to create staging buffer");
            return 0;
        }

        if (!VulkanCreateBuffer(Buffer->Size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &Buffer->Buffer, &Buffer->BufferMemory)) {
            SK_LOG_ERROR("Failed to create device buffer");
            return 0;
        }

        if (vkMapMemory(VulkanContext.Device, Buffer->StagingBufferMemory, 0, Buffer->Size, 0, &Buffer->MappedData) != VK_SUCCESS) {
            SK_LOG_ERROR("Failed to map buffer memory");
            return 0;
        }
    } else {
        if (!VulkanCreateBuffer(Buffer->Size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &Buffer->Buffer, &Buffer->BufferMemory)) {
            SK_LOG_ERROR("Failed to create device buffer");
            return 0;
        }

        if (vkMapMemory(VulkanContext.Device, Buffer->BufferMemory, 0, Buffer->Size, 0, &Buffer->MappedData) != VK_SUCCESS) {
            SK_LOG_ERROR("Failed to map buffer memory");
            return 0;
        }
    }

    if (Description->InitialContents != nullptr) {
        if (Description->InitialContentsSize > Buffer->Size) {
            SK_LOG_WARN("Initial contents size is greater thatn buffer size");
            return (FRHIResourceHandle)Buffer;
        }

        memcpy(Buffer->MappedData, Description->InitialContents, Description->InitialContentsSize);
        if (!VulkanSendBufferToGPU((FRHIResourceHandle)Buffer)) {
            SK_LOG_ERROR("Failed to send buffer to gpu");
            return 0;
        }
    }

    return (FRHIResourceHandle)Buffer;
}

FRHIResourceHandle VulkanCreateIndexBuffer(FRHIIndexBufferDescription *Description) {
    auto Buffer = (FVulkanBuffer *) malloc(sizeof(FVulkanBuffer));

    Buffer->UseStagingBuffer = Description->UseStagingBuffer;
    Buffer->ElementCount = Description->Count;
    Buffer->Size = Description->Count * sizeof(FUInt16);
    Buffer->Type = ERHIBufferType::INDEX;
    Buffer->Layout = {};

    if (Buffer->UseStagingBuffer) {
        if (!VulkanCreateBuffer(Buffer->Size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &Buffer->StagingBuffer, &Buffer->StagingBufferMemory)) {
            SK_LOG_ERROR("Failed to create staging buffer");
            return 0;
        }

        if (!VulkanCreateBuffer(Buffer->Size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &Buffer->Buffer, &Buffer->BufferMemory)) {
            SK_LOG_ERROR("Failed to create device buffer");
            return 0;
        }

        if (vkMapMemory(VulkanContext.Device, Buffer->StagingBufferMemory, 0, Buffer->Size, 0, &Buffer->MappedData) != VK_SUCCESS) {
            SK_LOG_ERROR("Failed to map buffer memory");
            return 0;
        }
    } else {
        if (!VulkanCreateBuffer(Buffer->Size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &Buffer->Buffer, &Buffer->BufferMemory)) {
            SK_LOG_ERROR("Failed to create device buffer");
            return 0;
        }

        if (vkMapMemory(VulkanContext.Device, Buffer->BufferMemory, 0, Buffer->Size, 0, &Buffer->MappedData) != VK_SUCCESS) {
            SK_LOG_ERROR("Failed to map buffer memory");
            return 0;
        }
    }

    if (Description->InitialContents != nullptr) {
        if (Description->InitialContentsSize > Buffer->Size) {
            SK_LOG_WARN("Initial contents size is greater thatn buffer size");
            return (FRHIResourceHandle)Buffer;
        }

        memcpy(Buffer->MappedData, Description->InitialContents, Description->InitialContentsSize);
        if (!VulkanSendBufferToGPU((FRHIResourceHandle)Buffer)) {
            SK_LOG_ERROR("Failed to send buffer to gpu");
            return 0;
        }
    }

    return (FRHIResourceHandle)Buffer;
}

void VulkanDestroyBuffer(FRHIResourceHandle *Resource) {
    auto Buffer = (FVulkanBuffer *)*Resource;

    if (Buffer->UseStagingBuffer) {
        vkFreeMemory(VulkanContext.Device, Buffer->StagingBufferMemory, nullptr);
        vkDestroyBuffer(VulkanContext.Device, Buffer->StagingBuffer, nullptr);
    }

    vkFreeMemory(VulkanContext.Device, Buffer->BufferMemory, nullptr);
    vkDestroyBuffer(VulkanContext.Device, Buffer->Buffer, nullptr);

    free(Buffer);
    *Resource = 0;
}

FRHIResourceHandle VulkanCreatePipelineLayout(FRHIPipelineLayoutDescription *Description) {
    auto Layout = (FVulkanPipelineLayout *)malloc(sizeof(FVulkanPipelineLayout));

    VkPipelineLayoutCreateInfo Info = {};
    Info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

    std::vector<VkDescriptorSetLayout> Layouts = {};
    Layouts.reserve(Description->DescriptorSetLayoutCount * sizeof(VkDescriptorSetLayout));

    for (FUInt32 Index = 0; Index < Description->DescriptorSetLayoutCount; Index++) {
        auto Layout = (FVulkanDescriptorSetLayout *)Description->DescriptorSetLayouts[Index];
        Layouts.push_back(Layout->Layout);
    }

    Info.setLayoutCount = static_cast<FUInt32>(Layouts.size());
    Info.pSetLayouts = Layouts.data();

    if (vkCreatePipelineLayout(VulkanContext.Device, &Info, nullptr, &Layout->Layout) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to create pipeline layout");
        return 0;
    }

    return (FRHIResourceHandle)Layout;
}

void VulkanDestroyPipelineLayout(FRHIResourceHandle *Resource) {
    auto Layout = (FVulkanPipelineLayout *)*Resource;
    vkDestroyPipelineLayout(VulkanContext.Device, Layout->Layout, nullptr);
    free(Layout);
    *Resource = 0;
}

FRHIResourceHandle VulkanCreateDescriptorSetLayout(FRHIDescriptorSetLayoutDescription *Description) {
    auto Layout = (FVulkanDescriptorSetLayout *)malloc(sizeof(FVulkanDescriptorSetLayout));

    VkDescriptorSetLayoutCreateInfo Info = {};
    Info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;

    std::vector<VkDescriptorSetLayoutBinding> Bindings;

    for (FUInt32 Index = 0; Index < Description->DescriptorCount; Index++) {
        auto Descriptor = Description->Descriptors[Index];
        VkDescriptorSetLayoutBinding Binding = {};
        Binding.binding = Descriptor.Binding;
        Binding.descriptorType = VulkanGetDescriptorType(Descriptor.Type);
        Binding.descriptorCount = Descriptor.Count;
        for (FUInt32 Index = 0; Index < Descriptor.ShaderStageCount; Index++)
            Binding.stageFlags |= VulkanGetShaderStage(Descriptor.Stages[Index]);

        Bindings.push_back(Binding);
    }

    Info.bindingCount = static_cast<FUInt32>(Bindings.size());
    Info.pBindings = Bindings.data();

    if (vkCreateDescriptorSetLayout(VulkanContext.Device, &Info, nullptr, &Layout->Layout) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to create descriptor set layout");
        return 0;
    }

    return (FRHIResourceHandle)Layout;
}

void VulkanDestroyDescriptorSetLayout(FRHIResourceHandle *Resource) {
    auto Layout = (FVulkanDescriptorSetLayout *)*Resource;
    vkDestroyDescriptorSetLayout(VulkanContext.Device, Layout->Layout, nullptr);
    free(Layout);
    *Resource = 0;
}

FRHIResourceHandle VulkanCreateGraphicsPipeline(FRHIGraphicsPipelineStateDescription *Description) {
    auto Pipeline = (FVulkanPipeline *)malloc(sizeof(FVulkanPipeline));
    Pipeline->BindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

    std::vector<VkFormat> ColorFormats;
    ColorFormats.reserve(Description->ColorFormatCount);

    for (FUInt32 Index = 0; Index < Description->ColorFormatCount; Index++) {
        ColorFormats.push_back(VulkanGetFormat(Description->ColorFormats[Index]));
    }

    VkPipelineRenderingCreateInfo RenderingInfo = {};
    RenderingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    RenderingInfo.colorAttachmentCount = static_cast<FUInt32>(ColorFormats.size());
    RenderingInfo.pColorAttachmentFormats = ColorFormats.data();
    RenderingInfo.depthAttachmentFormat = VulkanGetDepthFormat(Description->DepthStencilFormat);
    RenderingInfo.stencilAttachmentFormat = VulkanGetStencilFormat(Description->DepthStencilFormat);
    RenderingInfo.viewMask = 0x01;

    std::vector<VkPipelineShaderStageCreateInfo> ShaderStages = {};

    for (FUInt32 Index = 0; Index < Description->ShaderCount; Index++) {
        auto Shader = (FVulkanShader *)Description->Shaders[Index];
        VkPipelineShaderStageCreateInfo Info = {};
        Info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        Info.stage = Shader->Stage;
        Info.module = Shader->Shader;
        Info.pName = "main";

        ShaderStages.push_back(Info);
    }

    std::vector<VkVertexInputBindingDescription> VertexBindings = {};

    for (FUInt32 Index = 0; Index < Description->VertexInputBindingCount; Index++) {
        auto BindingDescription = Description->VertexInputBindings[Index];
        VkVertexInputBindingDescription Binding = {};
        Binding.binding = BindingDescription.Binding;
        Binding.stride = BindingDescription.Stride;
        Binding.inputRate = VulkanGetVertexInputRate(BindingDescription.InputRate);
        VertexBindings.push_back(Binding);
    }

    std::vector<VkVertexInputAttributeDescription> VertexAttributes = {};

    for (FUInt32 Index = 0; Index < Description->VertexInputAttributeCount; Index++) {
        auto AttributeDescription = Description->VertexInputAttributes[Index];
        VkVertexInputAttributeDescription Attribute = {};
        Attribute.location = AttributeDescription.Location;
        Attribute.binding = AttributeDescription.Binding;
        Attribute.format = VulkanGetFormat(AttributeDescription.Format);
        Attribute.offset = AttributeDescription.Offset;

        VertexAttributes.push_back(Attribute);
    }

    VkPipelineVertexInputStateCreateInfo VertexInput = {};
    VertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    VertexInput.vertexBindingDescriptionCount = static_cast<FUInt32>(VertexBindings.size());
    VertexInput.pVertexBindingDescriptions = VertexBindings.data();
    VertexInput.vertexAttributeDescriptionCount = static_cast<FUInt32>(VertexAttributes.size());
    VertexInput.pVertexAttributeDescriptions = VertexAttributes.data();

    VkPipelineInputAssemblyStateCreateInfo InputAssembly = {};
    InputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    InputAssembly.primitiveRestartEnable = false;
    InputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    VkPipelineTessellationStateCreateInfo Tesselation = {};

    VkPipelineViewportStateCreateInfo Viewport = {};
    Viewport.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    Viewport.viewportCount = 1;
    Viewport.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo Rasterizer = {};
    Rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    Rasterizer.depthClampEnable = VK_FALSE;
    Rasterizer.rasterizerDiscardEnable = VK_FALSE;
    Rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    Rasterizer.lineWidth = 1.0f;
    Rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    Rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    Rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo Multisampling = {};
    Multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    Multisampling.sampleShadingEnable = VK_FALSE;
    Multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkDynamicState DynamicStates[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
    };

    VkPipelineDynamicStateCreateInfo DynamicState = {};
    DynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    DynamicState.dynamicStateCount = 2;
    DynamicState.pDynamicStates = DynamicStates;

    VkPipelineColorBlendAttachmentState ColorBlendAttachment = {};
    ColorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    ColorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo ColorBlending = {};
    ColorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    ColorBlending.logicOpEnable = VK_FALSE;
    ColorBlending.logicOp = VK_LOGIC_OP_COPY;
    ColorBlending.attachmentCount = 1;
    ColorBlending.pAttachments = &ColorBlendAttachment;
    ColorBlending.blendConstants[0] = 0.0f;
    ColorBlending.blendConstants[1] = 0.0f;
    ColorBlending.blendConstants[2] = 0.0f;
    ColorBlending.blendConstants[3] = 0.0f;

    VkGraphicsPipelineCreateInfo Info = {};
    Info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    Info.pNext = &RenderingInfo;
    Info.stageCount = static_cast<FUInt32>(ShaderStages.size());
    Info.pStages = ShaderStages.data();
    Info.pVertexInputState = &VertexInput;
    Info.pInputAssemblyState = &InputAssembly;
    Info.pTessellationState = &Tesselation;
    Info.pViewportState = &Viewport;
    Info.pRasterizationState = &Rasterizer;
    Info.pMultisampleState = &Multisampling;
    Info.pDepthStencilState = nullptr;
    Info.pColorBlendState = &ColorBlending;
    Info.pDynamicState = &DynamicState;
    Info.layout = ((FVulkanPipelineLayout *)Description->Layout)->Layout;

    if (vkCreateGraphicsPipelines(VulkanContext.Device, VK_NULL_HANDLE, 1, &Info, nullptr, &Pipeline->Pipeline) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to create vulkan pipeline");
        return 0;
    }

    return (FRHIResourceHandle)Pipeline;
}

void VulkanDestroyPipeline(FRHIResourceHandle *Resource) {
    auto Pipeline = (FVulkanPipeline *)*Resource;
    vkDestroyPipeline(VulkanContext.Device, Pipeline->Pipeline, nullptr);
    free(Pipeline);
    *Resource = 0;
}

void VulkanSetBufferData(FRHIResourceHandle Buffer, void *Data, FUInt32 Size) {
    auto VulkanBuffer = (FVulkanBuffer *)Buffer;
    if (Size > VulkanBuffer->Size) {
        SK_LOG_WARN("Data is too large for buffer");
        return;
    }

    memcpy(VulkanBuffer->MappedData, Data, Size);
}

void VulkanMapBufferData(FRHIResourceHandle Buffer, void **Data, FUInt32 Size) {
    auto VulkanBuffer = (FVulkanBuffer *) Buffer;
    if (Size > VulkanBuffer->Size) {
        SK_LOG_WARN("Data is too large for buffer");
        return;
    }
    
    *Data = VulkanBuffer->MappedData;
}

bool VulkanSendBufferToGPU(FRHIResourceHandle Buffer) {
    auto VulkanBuffer = (FVulkanBuffer *) Buffer;
    if (VulkanBuffer->UseStagingBuffer) {
        if (!VulkanCopyBuffer(VulkanBuffer->Buffer, VulkanBuffer->StagingBuffer, VulkanBuffer->Size)) {
            SK_LOG_ERROR("Failed to set buffer data");
            return false;
        }
    }

    return true;
}

FRHIResourceHandle VulkanGetCurrentSwapchainTexture(FRHIResourceHandle Swapchain) {
    auto VulkanSwapchain = (FVulkanSwapchain *)Swapchain;

    return (FRHIResourceHandle)&VulkanSwapchain->Images[VulkanSwapchain->ImageIndex];
}

ERHIFormat VulkanGetTextureFormat(FRHIResourceHandle Texture) {
    auto VulkanTexture = (FVulkanTexture *)Texture;
    return VulkanGetRHIFormat(VulkanTexture->Format);
}

FRHIRenderArea VulkanGetTextureRenderArea(FRHIResourceHandle Texture) {
    auto VulkanTexture = (FVulkanTexture *) Texture;
    return VulkanTexture->RenderArea;
}

bool VulkanBeginCommandList(FRHIResourceHandle CommandList) {
    auto List = (FVulkanCommandList *) CommandList;

    if (vkResetCommandBuffer(List->Buffers[VulkanContext.CurrentFrame], 0) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to reset command buffer");
        return false;
    }

    VkCommandBufferBeginInfo BeginInfo = {};
    BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    if (vkBeginCommandBuffer(List->Buffers[VulkanContext.CurrentFrame], &BeginInfo) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to begin command buffer");
        return false;
    }

    return true;
}

bool VulkanEndCommandList(FRHIResourceHandle CommandList) {
    auto List = (FVulkanCommandList *)CommandList;

    if (vkEndCommandBuffer(List->Buffers[VulkanContext.CurrentFrame]) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to end command buffer");
        return false;
    }

    return true;
}

void VulkanCmdRenderImGuiDrawData(FRHIResourceHandle CommandList, ImDrawData *DrawData) {
    auto List = (FVulkanCommandList *)CommandList;

    ImGui_ImplVulkan_RenderDrawData(DrawData, List->Buffers[VulkanContext.CurrentFrame]);
}

void VulkanCmdResourceBarrier(FRHIResourceHandle CommandList, FRHIResourceBarrier *Barrier) {
    auto List = (FVulkanCommandList *)CommandList;

    switch (Barrier->Type) {
    case ERHIBarrierType::TRANSITION:
        VulkanTransitionBarrier(List, &Barrier->TransitionBarrier);
        break;
    default:
        SK_LOG_WARN("Unsupported resource barrier type");
        return;
    }
}

void VulkanCmdSetRenderTarget(FRHIResourceHandle CommandList, FRHIResourceHandle Target, FRHIRenderArea *RenderArea) {
    auto List = (FVulkanCommandList *)CommandList;
    auto VulkanTarget = (FVulkanTexture *)Target;

    VkRenderingAttachmentInfo AttachmentInfo = {};
    AttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    AttachmentInfo.clearValue.color.float32[0] = 1.0f;
    AttachmentInfo.clearValue.color.float32[1] = 0.0f;
    AttachmentInfo.clearValue.color.float32[2] = 1.0f;
    AttachmentInfo.clearValue.color.float32[3] = 1.0f;
    AttachmentInfo.imageView = VulkanTarget->ImageView;
    AttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    AttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    AttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    VkRenderingInfo RenderingInfo = {};
    RenderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    RenderingInfo.colorAttachmentCount = 1;
    RenderingInfo.pColorAttachments = &AttachmentInfo;
    RenderingInfo.viewMask = 0x01;
    RenderingInfo.layerCount = RenderArea->LayerCount;
    RenderingInfo.renderArea.extent = { RenderArea->X, RenderArea->Y };
    RenderingInfo.renderArea.extent = { RenderArea->Width, RenderArea->Height };
    vkCmdBeginRendering(List->Buffers[VulkanContext.CurrentFrame], &RenderingInfo);
}

void VulkanCmdUnsetRenderTarget(FRHIResourceHandle CommandList) {
    auto List = (FVulkanCommandList *)CommandList;


    vkCmdEndRendering(List->Buffers[VulkanContext.CurrentFrame]);
}

void VulkanCmdBindVertexBuffer(FRHIResourceHandle CommandList, FRHIResourceHandle Buffer, FUInt32 FirstVertex) {
    auto List = (FVulkanCommandList *)CommandList;
    auto VulkanBuffer = (FVulkanBuffer *) Buffer;

    VkDeviceSize Offset = FirstVertex;

    vkCmdBindVertexBuffers(List->Buffers[VulkanContext.CurrentFrame], 0, 1, &VulkanBuffer->Buffer, &Offset);
}

void VulkanCmdBindIndexBuffer(FRHIResourceHandle CommandList, FRHIResourceHandle Buffer) {
    auto List = (FVulkanCommandList *)CommandList;
    auto VulkanBuffer = (FVulkanBuffer *)Buffer;

    vkCmdBindIndexBuffer(List->Buffers[VulkanContext.CurrentFrame], VulkanBuffer->Buffer, 0, VK_INDEX_TYPE_UINT16);
}

void VulkanCmdDrawIndexed(FRHIResourceHandle CommandList, FUInt32 IndexCount, FUInt32 InstanceCount, FUInt32 FirstIndex, FSInt32 VertexOffset, FUInt32 FirstInstance) {
    auto List = (FVulkanCommandList *)CommandList;

    vkCmdDrawIndexed(List->Buffers[VulkanContext.CurrentFrame], IndexCount, InstanceCount, FirstIndex, VertexOffset, FirstInstance);
}

void VulkanCmdDrawInstanced(FRHIResourceHandle CommandList, FUInt32 VertexCount, FUInt32 InstanceCount, FUInt32 FirstVertex, FUInt32 FirstInstance) {
    auto List = (FVulkanCommandList *)CommandList;

    vkCmdDraw(List->Buffers[VulkanContext.CurrentFrame], VertexCount, InstanceCount, FirstVertex, FirstInstance);
}

void VulkanCmdSetViewport(FRHIResourceHandle CommandList, FFloat X, FFloat Y, FFloat Width, FFloat Height, FFloat MinDepth, FFloat MaxDepth) {
    auto List = (FVulkanCommandList *)CommandList;

    VkViewport Viewport = {};
    Viewport.x = X;
    Viewport.y = Y;
    Viewport.width = Width;
    Viewport.height = Height;
    Viewport.minDepth = MinDepth;
    Viewport.maxDepth = MaxDepth;

    vkCmdSetViewport(List->Buffers[VulkanContext.CurrentFrame], 0, 1, &Viewport);
}

void VulkanCmdSetScissor(FRHIResourceHandle CommandList, FSInt32 X, FSInt32 Y, FUInt32 Width, FUInt32 Height) {
    auto List = (FVulkanCommandList *)CommandList;

    VkRect2D Scissor = {};
    Scissor.extent.width = Width;
    Scissor.extent.height = Height;
    Scissor.offset.x = X;
    Scissor.offset.x = Y;

    vkCmdSetScissor(List->Buffers[VulkanContext.CurrentFrame], 0, 1, &Scissor);
}

void VulkanCmdBindPipeline(FRHIResourceHandle CommandList, FRHIResourceHandle Pipeline) {
    auto List = (FVulkanCommandList *)CommandList;
    auto VulkanPipeline = (FVulkanPipeline *) Pipeline;

    vkCmdBindPipeline(List->Buffers[VulkanContext.CurrentFrame], VulkanPipeline->BindPoint, VulkanPipeline->Pipeline);
}

void VulkanNotifySwapchainOfResize(FRHIResourceHandle Swapchain) {
    auto VulkanSwapchain = (FVulkanSwapchain *)Swapchain;

    VulkanSwapchain->FramebufferResized = true;
}