#include "External/Vulkan/Include/vulkan/vulkan_core.h"
#include "Renderer.h"
#include "VulkanPlatform.h"
#include <cstdint>
#include <cwchar>
#include <objidlbase.h>
#include <stdexcept>
#include <wingdi.h>

#ifdef SK_VULKAN

#include "RenderApi.h"
#include "Log.h"
#include "Window.h"

#include <vector>
#include <optional>
#include <set>
#include <string>
#include <limits>
#include <algorithm>
#include <fstream>
#include <iostream>

/*********************
 * Private Interface *
 *********************/

struct queue_family_indices
{
    std::optional<u32> GraphicsFamily;
    std::optional<u32> PresentFamily;
};

struct swap_chain_support_details
{
    VkSurfaceCapabilitiesKHR Capabilities;
    std::vector<VkSurfaceFormatKHR> Formats;
    std::vector<VkPresentModeKHR> PresentModes;
};

struct renderer_api
{
	window *Window;
	VkInstance Instance;
	VkDebugUtilsMessengerEXT DebugMessenger;
	VkPhysicalDevice PhysicalDevice;
	VkDevice Device;
	VkQueue GraphicsQueue;
	VkQueue PresentQueue;
	VkSurfaceKHR Surface;
	VkSwapchainKHR SwapChain;
	std::vector<VkImage> SwapChainImages;
	VkFormat SwapChainImageFormat;
	VkExtent2D SwapChainExtent;
	std::vector<VkImageView> SwapChainImageViews;
	VkPipelineLayout PipelineLayout;
	VkRenderPass RenderPass;
	VkPipeline GraphicsPipeline;
    std::vector<VkFramebuffer> SwapChainFrameBuffers;
    VkCommandPool CommandPool;
    std::vector<VkCommandBuffer> CommandBuffers;
    std::vector<VkSemaphore> ImageAvailableSemaphores;
    std::vector<VkSemaphore> RenderFinishedSemaphores;
    std::vector<VkFence> InFlightFences;
    u32 CurrentFrame = 0;
    bool FramebufferResized = false;
};

static renderer_api RendererApi;

void
CreateFramebuffers()
{
    RendererApi.SwapChainFrameBuffers.resize(RendererApi.SwapChainImageViews.size());

    for (u64 i = 0; i < RendererApi.SwapChainImageViews.size(); i++)
    {
        VkImageView Attachments[] = {
            RendererApi.SwapChainImageViews[i]
        };

        VkFramebufferCreateInfo FramebufferInfo = {};
        FramebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        FramebufferInfo.renderPass = RendererApi.RenderPass;
        FramebufferInfo.attachmentCount = 1;
        FramebufferInfo.pAttachments = Attachments;
        FramebufferInfo.width = RendererApi.SwapChainExtent.width;
        FramebufferInfo.height = RendererApi.SwapChainExtent.height;
        FramebufferInfo.layers = 1;

        if (vkCreateFramebuffer(RendererApi.Device, &FramebufferInfo, NULL, &RendererApi.SwapChainFrameBuffers[i]))
        {
            throw std::runtime_error("Failed to create framebuffer");
        }
    }
}

VkExtent2D
ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &Capabilities)
{
    if (Capabilities.currentExtent.width != std::numeric_limits<u32>::max())
    {
        return Capabilities.currentExtent;
    }
    else
    {
        i32 Width = RendererApi.Window->Width;
        i32 Height = RendererApi.Window->Height;

        VkExtent2D Extent = { (u32)Width, (u32)Height };

        Extent.width = std::clamp(Extent.width, Capabilities.minImageExtent.width, Capabilities.maxImageExtent.width);
        Extent.height = std::clamp(Extent.height, Capabilities.minImageExtent.height, Capabilities.maxImageExtent.height);

        return Extent;
    }
}

VkPresentModeKHR
ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> &AvailablePresentModes)
{
    for (const auto &AvailablePresentMode : AvailablePresentModes)
    {
        if (AvailablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            return AvailablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkSurfaceFormatKHR
ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &AvailableFormats)
{
    for (const auto &AvailableFormat : AvailableFormats)
    {
        if (AvailableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && AvailableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return AvailableFormat;
        }
    }

    return AvailableFormats[0];
}

swap_chain_support_details
QuerySwapChainSupport(VkPhysicalDevice Device)
{
    swap_chain_support_details Details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(Device, RendererApi.Surface, &Details.Capabilities);

    u32 FormatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(Device, RendererApi.Surface, &FormatCount, NULL);

    if (FormatCount != 0)
    {
        Details.Formats.resize(FormatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(Device, RendererApi.Surface, &FormatCount, Details.Formats.data());
    }

    u32 PresentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(Device, RendererApi.Surface, &PresentModeCount, Details.PresentModes.data());

    if (PresentModeCount != 0)
    {
        Details.PresentModes.resize(PresentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(Device, RendererApi.Surface, &PresentModeCount, Details.PresentModes.data());
    }

    return Details;
}

bool
QueueFamilyIndicesAreComplete(queue_family_indices *Indices)
{
    return Indices->GraphicsFamily.has_value() && Indices->PresentFamily.has_value();
}


std::vector<const char *>
GetRequiredExtensions()
{
    std::vector<const char *> Extensions;

#ifdef WIN32
    Extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
    Extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif

    if (EnableValidationLayers)
    {
        Extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return Extensions;
}

VKAPI_ATTR VkBool32 VKAPI_CALL
DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT MessageSeverity, VkDebugUtilsMessageTypeFlagsEXT MessageType, const VkDebugUtilsMessengerCallbackDataEXT *CallbackData, void *UserData)
{
    LogCoreTrace("Validation Layer: %s", CallbackData->pMessage);

    return VK_FALSE;
}

bool
CheckValidationLayerSupport()
{
    u32 LayerCount;
    vkEnumerateInstanceLayerProperties(&LayerCount, NULL);

    std::vector<VkLayerProperties> AvailableLayers(LayerCount);
    vkEnumerateInstanceLayerProperties(&LayerCount, AvailableLayers.data());

    for (const char *LayerName : ValidationLayers)
    {
        bool LayerFound = false;

        for (u32 i = 0; i < LayerCount; i++)
        {
            if (strcmp(LayerName, AvailableLayers[i].layerName))
            {
                LayerFound = true;
                break;
            }
        }

        if (!LayerFound)
        {
            return false;
        }
    }

    return true;
}

bool
CreateInstance()
{
    VkApplicationInfo AppInfo = {};
    AppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    AppInfo.pApplicationName = "Default App";
    AppInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    AppInfo.pEngineName = "Stak Engine";
    AppInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    AppInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo CreateInfo = {};
    CreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    CreateInfo.pApplicationInfo = &AppInfo;

    std::vector<const char *> Extensions = GetRequiredExtensions();
    CreateInfo.enabledExtensionCount = (u32)Extensions.size();
    CreateInfo.ppEnabledExtensionNames = Extensions.data();

    if (EnableValidationLayers && !CheckValidationLayerSupport())
    {
        return false;
    }

    VkDebugUtilsMessengerCreateInfoEXT DebugMessengerCreateInfo = {};

    if (EnableValidationLayers)
    {
        CreateInfo.enabledLayerCount = NumValidationLayers;
        CreateInfo.ppEnabledLayerNames = ValidationLayers;

        DebugMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        DebugMessengerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        DebugMessengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        DebugMessengerCreateInfo.pfnUserCallback = DebugCallback;

        CreateInfo.pNext = &DebugMessengerCreateInfo;
    }
    else
    {
        CreateInfo.enabledLayerCount = 0;

        CreateInfo.pNext = NULL;
    }

    if (vkCreateInstance(&CreateInfo, NULL, &RendererApi.Instance) != VK_SUCCESS)
    {
        return false;
    }

    return true;
}

VkResult
CreateDebugUtilsMessengerEXT(VkInstance Instance, const VkDebugUtilsMessengerCreateInfoEXT *CreateInfo, const VkAllocationCallbacks *Allocator, VkDebugUtilsMessengerEXT *DebugMessenger)
{
    auto Func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(Instance, "vkCreateDebugUtilsMessengerEXT");
    if (Func != NULL)
    {
        return Func(Instance, CreateInfo, Allocator, DebugMessenger);
    }
    else
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

bool
SetupDebugMessenger()
{
    if (!EnableValidationLayers)
    {
        return true;
    }

    VkDebugUtilsMessengerCreateInfoEXT CreateInfo = {};
    CreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    CreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    CreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    CreateInfo.pfnUserCallback = DebugCallback;

    if (CreateDebugUtilsMessengerEXT(RendererApi.Instance, &CreateInfo, NULL, &RendererApi.DebugMessenger) != VK_SUCCESS)
    {
        return false;
    }

    return true;
}

void
DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *pAllocator)
{
    auto Func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (Func != nullptr)
    {
        Func(instance, debugMessenger, pAllocator);
    }
}

queue_family_indices
FindQueueFamilies(VkPhysicalDevice Device)
{
    queue_family_indices Indices;
    u32 QueueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(Device, &QueueFamilyCount, NULL);

    std::vector<VkQueueFamilyProperties> QueueFamilies(QueueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(Device, &QueueFamilyCount, QueueFamilies.data());

    i32 i = 0;
    for (const auto &QueueFamily : QueueFamilies)
    {
        if (QueueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            Indices.GraphicsFamily = i;
        }

        VkBool32 PresentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(Device, i, RendererApi.Surface, &PresentSupport);

        if (PresentSupport)
        {
            Indices.PresentFamily = i;
        }

        if (QueueFamilyIndicesAreComplete(&Indices))
        {
            break;
        }

        i++;
    }

    return Indices;
}

void
CreateCommandPool()
{
    queue_family_indices QueueFamilyIndices = FindQueueFamilies(RendererApi.PhysicalDevice);

    VkCommandPoolCreateInfo PoolInfo = {};
    PoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    PoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    PoolInfo.queueFamilyIndex = QueueFamilyIndices.GraphicsFamily.value();

    if (vkCreateCommandPool(RendererApi.Device, &PoolInfo, NULL, &RendererApi.CommandPool) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create command pool");
    }
}

void
CreateCommandBuffer()
{
    RendererApi.CommandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo AllocInfo = {};
    AllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    AllocInfo.commandPool = RendererApi.CommandPool;
    AllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    AllocInfo.commandBufferCount = (u32)RendererApi.CommandBuffers.size();

    if (vkAllocateCommandBuffers(RendererApi.Device, &AllocInfo, RendererApi.CommandBuffers.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to allocate command buffer");
    }
}

void
RecordCommandBuffer(VkCommandBuffer CommandBuffer, u32 ImageIndex)
{
    VkCommandBufferBeginInfo BeginInfo = {};
    BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(RendererApi.CommandBuffers[RendererApi.CurrentFrame], &BeginInfo) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to start command buffer");
    }

    VkRenderPassBeginInfo RenderPassInfo = {};
    RenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    RenderPassInfo.renderPass = RendererApi.RenderPass;
    RenderPassInfo.framebuffer = RendererApi.SwapChainFrameBuffers[ImageIndex];
    RenderPassInfo.renderArea.offset = { 0, 0 };
    RenderPassInfo.renderArea.extent = RendererApi.SwapChainExtent;
    VkClearValue ClearColor = {{{ 0.0f, 0.0f, 0.0f, 1.0f }}};
    RenderPassInfo.clearValueCount = 1;
    RenderPassInfo.pClearValues = &ClearColor;
    vkCmdBeginRenderPass(RendererApi.CommandBuffers[RendererApi.CurrentFrame], &RenderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(RendererApi.CommandBuffers[RendererApi.CurrentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, RendererApi.GraphicsPipeline);

    VkViewport Viewport = {};
    Viewport.x = 0.0f;
    Viewport.y = 0.0f;
    Viewport.width = (f32)RendererApi.SwapChainExtent.width;
    Viewport.height = (f32)RendererApi.SwapChainExtent.height;
    Viewport.maxDepth = 1.0f;
    Viewport.minDepth = 0.0f;
    vkCmdSetViewport(RendererApi.CommandBuffers[RendererApi.CurrentFrame], 0, 1, &Viewport);

    VkRect2D Scissor = {};
    Scissor.offset = { 0, 0 };
    Scissor.extent = RendererApi.SwapChainExtent;
    vkCmdSetScissor(RendererApi.CommandBuffers[RendererApi.CurrentFrame], 0, 1, &Scissor);

    vkCmdDraw(RendererApi.CommandBuffers[RendererApi.CurrentFrame], 3, 1, 0, 0);

    vkCmdEndRenderPass(RendererApi.CommandBuffers[RendererApi.CurrentFrame]);
    if (vkEndCommandBuffer(RendererApi.CommandBuffers[RendererApi.CurrentFrame]) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to record command buffer");
    }
}

bool
CheckDeviceExtensionSupport(VkPhysicalDevice Device)
{
    u32 ExtensionCount;
    vkEnumerateDeviceExtensionProperties(Device, NULL, &ExtensionCount, NULL);

    std::vector<VkExtensionProperties> AvailableExtensions(ExtensionCount);
    vkEnumerateDeviceExtensionProperties(Device, NULL, &ExtensionCount, AvailableExtensions.data());

    std::set<std::string> RequiredExtensions(DeviceExtensions.begin(), DeviceExtensions.end());

    for (const auto &Extension : AvailableExtensions)
    { 
        RequiredExtensions.erase(Extension.extensionName);
    }

    return RequiredExtensions.empty();
}

bool
IsDeviceSuitable(VkPhysicalDevice Device)
{
    queue_family_indices Indices = FindQueueFamilies(Device);
    bool ExtensionsSupported = CheckDeviceExtensionSupport(Device);

    bool SwapChainAdequate = false;
    if (ExtensionsSupported)
    {
        swap_chain_support_details SwapChainSupport = QuerySwapChainSupport(Device);
        SwapChainAdequate = !SwapChainSupport.Formats.empty() && !SwapChainSupport.PresentModes.empty();
    }

    return QueueFamilyIndicesAreComplete(&Indices) && ExtensionsSupported && SwapChainAdequate;
}

bool
PickPhysicalDevice()
{
    u32 DeviceCount = 0;
    vkEnumeratePhysicalDevices(RendererApi.Instance, &DeviceCount, NULL);
    if (DeviceCount == 0)
    {
        return false;
    }
    std::vector<VkPhysicalDevice> Devices(DeviceCount);
    vkEnumeratePhysicalDevices(RendererApi.Instance, &DeviceCount, Devices.data());
    for (const VkPhysicalDevice &Device : Devices)
    {
        if (IsDeviceSuitable(Device))
        {
            RendererApi.PhysicalDevice = Device;
            return true;
        }
    }
    return false;
}

bool
CreateLogicalDevice()
{
    queue_family_indices Indices = FindQueueFamilies(RendererApi.PhysicalDevice);

    std::vector<VkDeviceQueueCreateInfo> QueueCreateInfos = {};
    std::set<u32> UniqueQueueFamilies = { Indices.GraphicsFamily.value(), Indices.PresentFamily.value() };

    f32 QueuePriority = 1.0f;
    for (uint32_t queueFamily : UniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo QueueCreateInfo{};
        QueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        QueueCreateInfo.queueFamilyIndex = queueFamily;
        QueueCreateInfo.queueCount = 1;
        QueueCreateInfo.pQueuePriorities = &QueuePriority;
        QueueCreateInfos.push_back(QueueCreateInfo);
    }

    VkPhysicalDeviceFeatures DeviceFeatures = {};

    VkDeviceCreateInfo CreateInfo = {};
    CreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    CreateInfo.queueCreateInfoCount = (u32)QueueCreateInfos.size();
    CreateInfo.pQueueCreateInfos = QueueCreateInfos.data();

    CreateInfo.pEnabledFeatures = &DeviceFeatures;

    CreateInfo.enabledExtensionCount = 0;

    CreateInfo.enabledExtensionCount = DeviceExtensions.size();
    CreateInfo.ppEnabledExtensionNames = DeviceExtensions.data();

    if (EnableValidationLayers)
    {
        CreateInfo.enabledLayerCount = NumValidationLayers;
        CreateInfo.ppEnabledLayerNames = ValidationLayers;
    }
    else
    {
        CreateInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(RendererApi.PhysicalDevice, &CreateInfo, nullptr, &RendererApi.Device) != VK_SUCCESS)
    {
        return false;
    }

    vkGetDeviceQueue(RendererApi.Device, Indices.GraphicsFamily.value(), 0, &RendererApi.GraphicsQueue);
    vkGetDeviceQueue(RendererApi.Device, Indices.GraphicsFamily.value(), 0, &RendererApi.PresentQueue);

    return true;
}

bool
CreateSurface()
{
#ifdef SK_WINDOWS
    VkWin32SurfaceCreateInfoKHR CreateInfo = {};
    CreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    CreateInfo.hwnd = RendererApi.Window->Handle;
    CreateInfo.hinstance = Platform.Instance;

    if (vkCreateWin32SurfaceKHR(RendererApi.Instance, &CreateInfo, NULL, &RendererApi.Surface) != VK_SUCCESS)
    {
        return false;
    }
#endif

    return true;
}

bool
CreateSwapChain()
{
    swap_chain_support_details SwapChainSupport = QuerySwapChainSupport(RendererApi.PhysicalDevice);

    VkSurfaceFormatKHR SurfaceFormat = ChooseSwapSurfaceFormat(SwapChainSupport.Formats);
    VkPresentModeKHR PresentMode = ChooseSwapPresentMode(SwapChainSupport.PresentModes);
    VkExtent2D Extent = ChooseSwapExtent(SwapChainSupport.Capabilities);

    u32 ImageCount = SwapChainSupport.Capabilities.minImageCount + 1;

    if (SwapChainSupport.Capabilities.maxImageCount > 0 && ImageCount > SwapChainSupport.Capabilities.maxImageCount)
    {
        ImageCount = SwapChainSupport.Capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR CreateInfo = {};
    CreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    CreateInfo.surface = RendererApi.Surface;
    CreateInfo.minImageCount = ImageCount;
    CreateInfo.imageFormat = SurfaceFormat.format;
    CreateInfo.imageColorSpace = SurfaceFormat.colorSpace;
    CreateInfo.imageExtent = Extent;
    CreateInfo.imageArrayLayers = 1;
    // VK_IMAGE_USAGE_TRANSFER_DST_BIT for post processing
    CreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    queue_family_indices Indices = FindQueueFamilies(RendererApi.PhysicalDevice);
    u32 QueueFamilyIndices[] = { Indices.GraphicsFamily.value(), Indices.PresentFamily.value() };

    if (Indices.GraphicsFamily != Indices.PresentFamily)
    {
        CreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        CreateInfo.queueFamilyIndexCount = 2;
        CreateInfo.pQueueFamilyIndices = QueueFamilyIndices;
    }
    else
    {
        CreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        CreateInfo.queueFamilyIndexCount = 0;
        CreateInfo.pQueueFamilyIndices = NULL;
    }

    CreateInfo.preTransform = SwapChainSupport.Capabilities.currentTransform;
    CreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    CreateInfo.presentMode = PresentMode;
    CreateInfo.clipped = VK_TRUE;
    CreateInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(RendererApi.Device, &CreateInfo, NULL, &RendererApi.SwapChain) != VK_SUCCESS)
    {
        return false;
    }

    vkGetSwapchainImagesKHR(RendererApi.Device, RendererApi.SwapChain, &ImageCount, NULL);
    RendererApi.SwapChainImages.resize(ImageCount);
    vkGetSwapchainImagesKHR(RendererApi.Device, RendererApi.SwapChain, &ImageCount, RendererApi.SwapChainImages.data());

    RendererApi.SwapChainImageFormat = SurfaceFormat.format;
    RendererApi.SwapChainExtent = Extent;

    return true;
}

bool
CreateImageViews()
{
    RendererApi.SwapChainImageViews.resize(RendererApi.SwapChainImages.size());

    for (u64 i = 0; i < RendererApi.SwapChainImages.size(); i++)
    {
        VkImageViewCreateInfo CreateInfo = {};
        CreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        CreateInfo.image = RendererApi.SwapChainImages[i];
        CreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        CreateInfo.format = RendererApi.SwapChainImageFormat;
        CreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        CreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        CreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        CreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        CreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        CreateInfo.subresourceRange.baseMipLevel = 0;
        CreateInfo.subresourceRange.levelCount = 1;
        CreateInfo.subresourceRange.baseArrayLayer = 0;
        CreateInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(RendererApi.Device, &CreateInfo, NULL, &RendererApi.SwapChainImageViews[i]) != VK_SUCCESS)
        {
            return false;
        }
    }

    return true;
}

// TODO: Temporary
std::vector<char>
ReadFile(const std::string &FileName)
{
    std::ifstream File(FileName, std::ios::ate | std::ios::binary);

    if (!File.is_open())
    {
        LogCoreError("Failed to open file: %s", FileName.c_str());
        throw std::runtime_error("Failed to open file");
    }

    u64 FileSize = (u64)File.tellg();
    std::vector<char> Buffer(FileSize);
    File.seekg(0);
    File.read(Buffer.data(), FileSize);
    File.close();
    return Buffer;
}

VkShaderModule
CreateShaderModule(const std::vector<char> &Code)
{
    VkShaderModuleCreateInfo CreateInfo = {};
    CreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    CreateInfo.codeSize = Code.size();
    CreateInfo.pCode = (const u32 *)Code.data();

    VkShaderModule ShaderModule;
    if (vkCreateShaderModule(RendererApi.Device, &CreateInfo, NULL, &ShaderModule) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create shader module");
    }

    return ShaderModule;
}

void
CreateGraphicsPipeline()
{
    std::vector<char> VertexShaderCode = ReadFile(SHADER_DIR "BasicShader.vert.spv");
    std::vector<char> FragmentShaderCode = ReadFile(SHADER_DIR "BasicShader.frag.spv");

    VkShaderModule VertexShaderModule = CreateShaderModule(VertexShaderCode);
    VkShaderModule FragmentShaderModule = CreateShaderModule(FragmentShaderCode);

    VkPipelineShaderStageCreateInfo VertexShaderStageInfo = {};
    VertexShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    VertexShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;

    VertexShaderStageInfo.module = VertexShaderModule;
    VertexShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo FragmentShaderStageInfo = {};
    FragmentShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    FragmentShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    FragmentShaderStageInfo.module = FragmentShaderModule;
    FragmentShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo ShaderStages[] =
    {
        VertexShaderStageInfo,
        FragmentShaderStageInfo
    };

    VkPipelineVertexInputStateCreateInfo VertexInputInfo{};
    VertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    VertexInputInfo.vertexBindingDescriptionCount = 0;
    VertexInputInfo.vertexAttributeDescriptionCount = 0;

    VkPipelineInputAssemblyStateCreateInfo InputAssembly = {};
    InputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    InputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    InputAssembly.primitiveRestartEnable = VK_FALSE;

    VkPipelineViewportStateCreateInfo ViewportState = {};
    ViewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    ViewportState.viewportCount = 1;
    ViewportState.scissorCount = 1;

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

    VkPipelineColorBlendAttachmentState ColorBlendAttachment = {};
    ColorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;;
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
    std::vector<VkDynamicState> DynamicStates =
    {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };
    VkPipelineDynamicStateCreateInfo DynamicState = {};
    DynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    DynamicState.dynamicStateCount = (u32)DynamicStates.size();
    DynamicState.pDynamicStates = DynamicStates.data();

    VkPipelineLayoutCreateInfo PipelineLayoutInfo = {};
    PipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    PipelineLayoutInfo.setLayoutCount = 0;
    PipelineLayoutInfo.pushConstantRangeCount = 0;

    if (vkCreatePipelineLayout(RendererApi.Device, &PipelineLayoutInfo, NULL, &RendererApi.PipelineLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create pipeline layout");
    }

    VkGraphicsPipelineCreateInfo PipelineInfo = {};
    PipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    PipelineInfo.stageCount = 2;
    PipelineInfo.pStages = ShaderStages;

    PipelineInfo.pVertexInputState = &VertexInputInfo;
    PipelineInfo.pInputAssemblyState = &InputAssembly;
    PipelineInfo.pViewportState = &ViewportState;
    PipelineInfo.pRasterizationState = &Rasterizer;
    PipelineInfo.pMultisampleState = &Multisampling;
    PipelineInfo.pDepthStencilState = NULL;
    PipelineInfo.pColorBlendState = &ColorBlending;
    PipelineInfo.pDynamicState = &DynamicState;

    PipelineInfo.layout = RendererApi.PipelineLayout;

    PipelineInfo.renderPass = RendererApi.RenderPass;
    PipelineInfo.subpass = 0;

    PipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    PipelineInfo.basePipelineIndex = -1;

    if (vkCreateGraphicsPipelines(RendererApi.Device, VK_NULL_HANDLE, 1, &PipelineInfo, NULL, &RendererApi.GraphicsPipeline) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create graphics pipeline");
    }

    vkDestroyShaderModule(RendererApi.Device, FragmentShaderModule, NULL);
    vkDestroyShaderModule(RendererApi.Device, VertexShaderModule, NULL);
}

void 
CreateRenderPass()
{
    VkAttachmentDescription ColorAttachment = {};
    ColorAttachment.format = RendererApi.SwapChainImageFormat;
    ColorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;

    ColorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    ColorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    ColorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    ColorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    ColorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    ColorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference ColorAttachmentRef = {};
    ColorAttachmentRef.attachment = 0;
    ColorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription Subpass = {};
    Subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

    Subpass.colorAttachmentCount = 1;
    Subpass.pColorAttachments = &ColorAttachmentRef;

    VkRenderPassCreateInfo RenderPassInfo = {};
    RenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    RenderPassInfo.attachmentCount = 1;
    RenderPassInfo.pAttachments = &ColorAttachment;
    RenderPassInfo.subpassCount = 1;
    RenderPassInfo.pSubpasses = &Subpass;

    VkSubpassDependency Dependency = {};
    Dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    Dependency.dstSubpass = 0;

    Dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    Dependency.srcAccessMask = 0;
    
    Dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    Dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    RenderPassInfo.dependencyCount = 1;
    RenderPassInfo.pDependencies = &Dependency;

    if (vkCreateRenderPass(RendererApi.Device, &RenderPassInfo, NULL, &RendererApi.RenderPass) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create render pass");
    }
}

void
CreateSyncObjects()
{
    RendererApi.ImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    RendererApi.RenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    RendererApi.InFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo SemaphoreInfo = {};
    SemaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo FenceInfo = {};
    FenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    FenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (u64 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        if (vkCreateSemaphore(RendererApi.Device, &SemaphoreInfo, NULL, &RendererApi.ImageAvailableSemaphores[i]) != VK_SUCCESS || 
                vkCreateSemaphore(RendererApi.Device, &SemaphoreInfo, NULL, &RendererApi.RenderFinishedSemaphores[i]) != VK_SUCCESS || 
                vkCreateFence(RendererApi.Device, &FenceInfo, NULL, &RendererApi.InFlightFences[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create semaphores");
        }
    }
}

void
CleanupSwapChain()
{
    for (u64 i = 0; i < RendererApi.SwapChainFrameBuffers.size(); i++)
    {
        vkDestroyFramebuffer(RendererApi.Device, RendererApi.SwapChainFrameBuffers[i], NULL);
    }

    for (u64 i = 0; i < RendererApi.SwapChainImageViews.size(); i++)
    {
        vkDestroyImageView(RendererApi.Device, RendererApi.SwapChainImageViews[i], NULL);
    }

    vkDestroySwapchainKHR(RendererApi.Device, RendererApi.SwapChain, NULL);
}

void
RecreateSwapChain()
{
    window_size_data Size = GetWindowSize(RendererApi.Window);
    while (Size.Width == 0 || Size.Height == 0)
    {
        Size = GetWindowSize(RendererApi.Window);
        UpdateWindow(RendererApi.Window);
    }

    vkDeviceWaitIdle(RendererApi.Device);

    CleanupSwapChain();

    CreateSwapChain();
    CreateImageViews();
    CreateFramebuffers();
}

/********************
 * Public Interface *
 ********************/

void
WaitForDevice()
{
    vkDeviceWaitIdle(RendererApi.Device);
}

void
DrawFrame()
{
    vkWaitForFences(RendererApi.Device, 1, &RendererApi.InFlightFences[RendererApi.CurrentFrame], VK_TRUE, UINT64_MAX);

    u32 ImageIndex;
    VkResult Result = vkAcquireNextImageKHR(RendererApi.Device, RendererApi.SwapChain, UINT64_MAX, RendererApi.ImageAvailableSemaphores[RendererApi.CurrentFrame], VK_NULL_HANDLE, &ImageIndex);

    if (Result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        RecreateSwapChain();
        return;
    }
    else if (Result != VK_SUCCESS && Result != VK_SUBOPTIMAL_KHR)
    {
        throw std::runtime_error("Failed to acquire swap chain image");
    }

    vkResetFences(RendererApi.Device, 1, &RendererApi.InFlightFences[RendererApi.CurrentFrame]);

    vkResetCommandBuffer(RendererApi.CommandBuffers[RendererApi.CurrentFrame], 0);
    RecordCommandBuffer(RendererApi.CommandBuffers[RendererApi.CurrentFrame], ImageIndex);

    VkSubmitInfo SubmitInfo = {};
    SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore WaitSemaphores[] = { RendererApi.ImageAvailableSemaphores[RendererApi.CurrentFrame] };
    VkPipelineStageFlags WaitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    SubmitInfo.waitSemaphoreCount = 1;
    SubmitInfo.pWaitSemaphores = WaitSemaphores;
    SubmitInfo.pWaitDstStageMask = WaitStages;

    SubmitInfo.commandBufferCount = 1;
    SubmitInfo.pCommandBuffers = &RendererApi.CommandBuffers[RendererApi.CurrentFrame];

    VkSemaphore SignalSemaphores[] = { RendererApi.RenderFinishedSemaphores[RendererApi.CurrentFrame] };
    SubmitInfo.signalSemaphoreCount = 1;
    SubmitInfo.pSignalSemaphores = SignalSemaphores;
    
    if (vkQueueSubmit(RendererApi.GraphicsQueue, 1, &SubmitInfo, RendererApi.InFlightFences[RendererApi.CurrentFrame]) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to submit draw command");
    }

    VkPresentInfoKHR PresentInfo = {};
    PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    PresentInfo.waitSemaphoreCount = 1;
    PresentInfo.pWaitSemaphores = SignalSemaphores;

    VkSwapchainKHR SwapChains[] = { RendererApi.SwapChain };
    PresentInfo.swapchainCount = 1;
    PresentInfo.pSwapchains = SwapChains;
    PresentInfo.pImageIndices = &ImageIndex;

    Result = vkQueuePresentKHR(RendererApi.PresentQueue, &PresentInfo);

    if (Result == VK_ERROR_OUT_OF_DATE_KHR || Result == VK_SUBOPTIMAL_KHR || RendererApi.FramebufferResized)
    {
        RendererApi.FramebufferResized = false;
        RecreateSwapChain();
    }
    else if (Result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to present swap chain image");
    }

    RendererApi.CurrentFrame = (RendererApi.CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void
RenderApiInit(window *Window)
{
    RendererApi.Window = Window;
    if (!CreateInstance())
    {
        LogCoreError("Failed to create vulkan instance");
    }
    if (!SetupDebugMessenger())
    {
        LogCoreError("Failed to setup vulkan debug messenger");
    }
    if (!CreateSurface())
    {
        LogCoreError("Failed to create window surface");
    }
    if (!PickPhysicalDevice())
    {
        LogCoreError("Failed to pick physical rendering device");
    }
    if (!CreateLogicalDevice())
    {
        LogCoreError("Failed to create logical device");
    }
    if (!CreateSwapChain())
    {
        LogCoreError("Failed to create swap chain");
    }
    if (!CreateImageViews())
    {
        LogCoreError("Failed to create image views");
    }
    CreateRenderPass();
    CreateGraphicsPipeline();
    CreateFramebuffers();
    CreateCommandPool();
    CreateCommandBuffer();
    CreateSyncObjects();
    LogCoreTrace("Vulkan Initialized");
}

void
RenderApiShutdown()
{
    CleanupSwapChain();

    vkDestroyPipeline(RendererApi.Device, RendererApi.GraphicsPipeline, NULL);
    vkDestroyPipelineLayout(RendererApi.Device, RendererApi.PipelineLayout, NULL);

    vkDestroyRenderPass(RendererApi.Device, RendererApi.RenderPass, NULL);

    for (u64 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        vkDestroySemaphore(RendererApi.Device, RendererApi.ImageAvailableSemaphores[i], NULL);
        vkDestroySemaphore(RendererApi.Device, RendererApi.RenderFinishedSemaphores[i], NULL);
        vkDestroyFence(RendererApi.Device, RendererApi.InFlightFences[i], NULL);
    }

    vkDestroyCommandPool(RendererApi.Device, RendererApi.CommandPool, NULL);

    vkDestroyDevice(RendererApi.Device, NULL);

    if (EnableValidationLayers)
    {
        DestroyDebugUtilsMessengerEXT(RendererApi.Instance, RendererApi.DebugMessenger, NULL);
    }

    vkDestroySurfaceKHR(RendererApi.Instance, RendererApi.Surface, NULL);
    vkDestroyInstance(RendererApi.Instance, NULL);
}

void
RenderApiSwapBuffers()
{
}

void
RenderApiSetClearColor(f32 Red, f32 Green, f32 Blue, f32 Alpha)
{
}

void
RenderApiSetViewport(i32 X, i32 Y, i32 Width, i32 Height)
{
    RendererApi.FramebufferResized = true;
}

void
RenderApiSetLineWidth(f32 Width)
{
}

void
RenderApiBind()
{
}

void
RenderApiClear()
{
}

void
RenderApiDrawIndexed(const vertex_array *VertexArray, u32 Count)
{
}

void
RenderApiDrawLines(const vertex_array *VertexArray, u32 Count)
{
}

#endif
