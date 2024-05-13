#include "VulkanPlatform.h"

#ifdef SK_VULKAN

#include "RenderApi.h"
#include "Log.h"
#include "Window.h"
#include "Texture.h"
#include "UniformBuffer.h"

#include <vector>
#include <optional>
#include <set>
#include <string>
#include <limits>
#include <algorithm>
#include <fstream>
#include <iostream>

///////////////////////
// Private Interface //
///////////////////////

// TODO
context *
CreateContext(const window *Window) { return NULL; }

void
DestroyContext(context **Context) {}

void
MakeContextCurrent(const context *Context) {}

void
BindShader(const shader *Shader) {}

u32
GetIndexBufferIndexCount(const index_buffer *IndexBuffer) { return 0; }

void
BindVertexArray(const vertex_array *VertexArray) {}

const index_buffer *
GetVertexArrayIndexBuffer(const vertex_array *VertexArray) { return NULL; }

void
PlatformInitExtensions() {}

shader *
CreateShader(const char *FilePath) { return NULL; }

void
SwapContextBuffers(const context *Context) {}

void
DestroyShader(shader **Shader) {}

vertex_buffer *
CreateVertexBuffer(u32 DontRemember) { return NULL; }

void
DestroyVertexBuffer(vertex_buffer **VertexBuffer) {}

void
SetVertexBufferData(vertex_buffer *VertexBuffer, const void *Data, u32 Size) {}

void
SetVertexBufferLayout(vertex_buffer *VertexBuffer, buffer_layout **BufferLayout) {}

index_buffer *
CreateIndexBuffer(u32 *Indices, u32 Count) { return NULL; }

vertex_array *
CreateVertexArray() { return NULL; }

void
DestroyVertexArray(vertex_array **VertexArray) {}

void
AddVertexBufferToVertexArray(vertex_array *VertexArray, const vertex_buffer *VertexBuffer) {}

void
SetVertexArrayIndexBuffer(vertex_array *VertexArray, index_buffer **IndexBuffer) {}

texture2d *
CreateTexture2D(const texture_specification *Spec) { return NULL; }

void
DestroyTexture2D(texture2d **Tex) {}

void
SetTexture2DData(texture2d *Tex, void *Data, u32 Size) {}

void
BindTexture2D(const texture2d *Tex, u32 Slot) {}

bool
CompareTexture2D(const texture2d *Tex1, const texture2d *Tex2) { return false; }

uniform_buffer *
CreateUniformBuffer(u32 Something, u32 Something2) { return NULL; }

void
DestroyUniformBuffer(uniform_buffer **Buffer) {}

void
SetUniformBufferData(uniform_buffer *Buffer, const void *Data, u32 Something1, u32 Something2) {}

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

static window *RenderApiWindow;

static VkInstance Instance;
static VkDebugUtilsMessengerEXT DebugMessenger;
static VkPhysicalDevice PhysicalDevice;
static VkDevice Device;
static VkQueue GraphicsQueue;
static VkQueue PresentQueue;
static VkSurfaceKHR Surface;
static VkSwapchainKHR SwapChain;
static std::vector<VkImage> SwapChainImages;
static VkFormat SwapChainImageFormat;
static VkExtent2D SwapChainExtent;
static std::vector<VkImageView> SwapChainImageViews;
static VkPipelineLayout PipelineLayout;

VkExtent2D
ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &Capabilities)
{
    if (Capabilities.currentExtent.width != std::numeric_limits<u32>::max())
    {
        return Capabilities.currentExtent;
    }
    else
    {
        i32 Width = RenderApiWindow->Width;
        i32 Height = RenderApiWindow->Height;

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

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(Device, Surface, &Details.Capabilities);

    u32 FormatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(Device, Surface, &FormatCount, NULL);

    if (FormatCount != 0)
    {
        Details.Formats.resize(FormatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(Device, Surface, &FormatCount, Details.Formats.data());
    }

    u32 PresentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(Device, Surface, &PresentModeCount, Details.PresentModes.data());

    if (PresentModeCount != 0)
    {
        Details.PresentModes.resize(PresentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(Device, Surface, &PresentModeCount, Details.PresentModes.data());
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

    if (vkCreateInstance(&CreateInfo, NULL, &Instance) != VK_SUCCESS)
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

    if (CreateDebugUtilsMessengerEXT(Instance, &CreateInfo, NULL, &DebugMessenger) != VK_SUCCESS)
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
        vkGetPhysicalDeviceSurfaceSupportKHR(Device, i, Surface, &PresentSupport);

        if (PresentSupport) {
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
    vkEnumeratePhysicalDevices(Instance, &DeviceCount, NULL);
    if (DeviceCount == 0)
    {
        return false;
    }
    std::vector<VkPhysicalDevice> Devices(DeviceCount);
    vkEnumeratePhysicalDevices(Instance, &DeviceCount, Devices.data());
    for (const VkPhysicalDevice &Device : Devices)
    {
        if (IsDeviceSuitable(Device))
        {
            PhysicalDevice = Device;
            return true;
        }
    }
    return false;
}

bool
CreateLogicalDevice()
{
    queue_family_indices Indices = FindQueueFamilies(PhysicalDevice);

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

    if (vkCreateDevice(PhysicalDevice, &CreateInfo, nullptr, &Device) != VK_SUCCESS)
    {
        return false;
    }

    vkGetDeviceQueue(Device, Indices.GraphicsFamily.value(), 0, &GraphicsQueue);
    vkGetDeviceQueue(Device, Indices.GraphicsFamily.value(), 0, &PresentQueue);

    return true;
}

bool
CreateSurface()
{
#ifdef SK_WINDOWS
    VkWin32SurfaceCreateInfoKHR CreateInfo = {};
    CreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    CreateInfo.hwnd = RenderApiWindow->Handle;
    CreateInfo.hinstance = Platform.Instance;

    if (vkCreateWin32SurfaceKHR(Instance, &CreateInfo, NULL, &Surface) != VK_SUCCESS)
    {
        return false;
    }
#endif

    return true;
}

bool
CreateSwapChain()
{
    swap_chain_support_details SwapChainSupport = QuerySwapChainSupport(PhysicalDevice);

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
    CreateInfo.surface = Surface;
    CreateInfo.minImageCount = ImageCount;
    CreateInfo.imageFormat = SurfaceFormat.format;
    CreateInfo.imageColorSpace = SurfaceFormat.colorSpace;
    CreateInfo.imageExtent = Extent;
    CreateInfo.imageArrayLayers = 1;
    // VK_IMAGE_USAGE_TRANSFER_DST_BIT for post processing
    CreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    queue_family_indices Indices = FindQueueFamilies(PhysicalDevice);
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

    if (vkCreateSwapchainKHR(Device, &CreateInfo, NULL, &SwapChain) != VK_SUCCESS)
    {
        return false;
    }

    vkGetSwapchainImagesKHR(Device, SwapChain, &ImageCount, NULL);
    SwapChainImages.resize(ImageCount);
    vkGetSwapchainImagesKHR(Device, SwapChain, &ImageCount, SwapChainImages.data());

    SwapChainImageFormat = SurfaceFormat.format;
    SwapChainExtent = Extent;

    return true;
}

bool
CreateImageViews()
{
    SwapChainImageViews.resize(SwapChainImages.size());

    for (u64 i = 0; i < SwapChainImages.size(); i++)
    {
        VkImageViewCreateInfo CreateInfo = {};
        CreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        CreateInfo.image = SwapChainImages[i];
        CreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        CreateInfo.format = SwapChainImageFormat;
        CreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        CreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        CreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        CreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        CreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        CreateInfo.subresourceRange.baseMipLevel = 0;
        CreateInfo.subresourceRange.levelCount = 1;
        CreateInfo.subresourceRange.baseArrayLayer = 0;
        CreateInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(Device, &CreateInfo, NULL, &SwapChainImageViews[i]) != VK_SUCCESS)
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
        LogCoreError("Failed to open file: %s", FileName);
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
    if (vkCreateShaderModule(Device, &CreateInfo, NULL, &ShaderModule) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create shader module");
    }

    return ShaderModule;
}

void
CreateGraphicsPipeline()
{
    std::vector<char> VertexShaderCode = ReadFile(SHADER_DIR "QuadShader.vert.spv");
    std::vector<char> FragmentShaderCode = ReadFile(SHADER_DIR "QuadShader.frag.spv");

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

    if (vkCreatePipelineLayout(Device, &PipelineLayoutInfo, NULL, &PipelineLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create pipeline layout");
    }

    vkDestroyShaderModule(Device, FragmentShaderModule, NULL);
    vkDestroyShaderModule(Device, VertexShaderModule, NULL);
}

//////////////////////
// Public Interface //
//////////////////////

void
RenderApiInit(window *Window)
{
    RenderApiWindow = Window;
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

    CreateGraphicsPipeline();

    LogCoreTrace("Vulkan Initialized");
}

void
RenderApiShutdown()
{
    vkDestroyPipelineLayout(Device, PipelineLayout, NULL);

    for (auto ImageView : SwapChainImageViews)
    {
        vkDestroyImageView(Device, ImageView, NULL);
    }

    vkDestroySwapchainKHR(Device, SwapChain, NULL);

    vkDestroyDevice(Device, NULL);

    if (EnableValidationLayers)
    {
        DestroyDebugUtilsMessengerEXT(Instance, DebugMessenger, NULL);
    }

    vkDestroySurfaceKHR(Instance, Surface, NULL);

    vkDestroyInstance(Instance, NULL);
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
