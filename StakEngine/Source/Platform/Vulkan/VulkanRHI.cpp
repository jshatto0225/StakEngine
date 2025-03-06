#include "VulkanRHI.h"

#include "Log.h"
#include "Application.h"
#include "VulkanCommandContext.h"
#include "VulkanShader.h"
#include "VulkanBuffer.h"
#include "VulkanPipeline.h"
#include "VulkanDescriptorSetLayout.h"
#include "VulkanViewport.h"
#include "VulkanTexture.h"

#include <set>

#ifdef SK_GLFW
#include "GLFWWindow.h"
#endif

VKAPI_ATTR VkBool32 VKAPI_CALL VkMessenger(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT *callback_data, void *user_data) {
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

VkResult CreateDebugMessenger(VkInstance Instance, const VkDebugUtilsMessengerCreateInfoEXT *Info, const VkAllocationCallbacks *Allocator, VkDebugUtilsMessengerEXT *Messenger) {
    auto Func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(Instance, "vkCreateDebugUtilsMessengerEXT");
    if (Func != nullptr) {
        return Func(Instance, Info, Allocator, Messenger);
    } else {
        SK_LOG_ERROR("Extension not present");
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void DestroyDebugMessenger(VkInstance Instance, VkDebugUtilsMessengerEXT Messenger, const VkAllocationCallbacks *Allocator) {
    auto Func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(Instance, "vkDestroyDebugUtilsMessengerEXT");
    if (Func != nullptr) {
        Func(Instance, Messenger, Allocator);
    }
}

FSInt32 FindGraphicsQueueFamiliy(VkPhysicalDevice GPU) {
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

VulkanSwapchainSupport GetSwapchainSupport(VkPhysicalDevice Device, VkSurfaceKHR Surface) {
    VulkanSwapchainSupport Support = {};

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(Device, Surface, &Support.Capabilities);

    FUInt32 FormatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(Device, Surface, &FormatCount, nullptr);
    Support.Formats.resize(FormatCount);

    vkGetPhysicalDeviceSurfaceFormatsKHR(Device, Surface, &FormatCount, Support.Formats.data());

    FUInt32 PresentModeCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(Device, Surface, &PresentModeCount, nullptr);
    Support.PresentModes.resize(PresentModeCount);

    vkGetPhysicalDeviceSurfacePresentModesKHR(Device, Surface, &PresentModeCount, Support.PresentModes.data());

    return Support;
}

VkImageView VulkanCreateImageView(VkDevice Device, VkImage Image, VkFormat Format) {
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

    VkImageView ImageView;
    CHECK_VK_ERR(vkCreateImageView(Device, &ViewInfo, nullptr, &ImageView), "Failed to create image view");

    return ImageView;
}

VkAccessFlags GetVulkanAccessMask(ERHIResourceState State) {
    switch (State) {
    case ERHIResourceState::RENDER_TARGET:
        return VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    case ERHIResourceState::PRESENT:
        return 0;
    default:
        return 0;
    }
}

VkImageLayout GetVulkanImageLayout(ERHIResourceState State) {
    switch (State) {
    case ERHIResourceState::RENDER_TARGET:
        return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    case ERHIResourceState::PRESENT:
        return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    default:
        return VK_IMAGE_LAYOUT_UNDEFINED;
    }
}

EVulkanQueue GetVulkanQueue(ERHIResourceState State) {
    switch (State) {
    case ERHIResourceState::RENDER_TARGET:
        return EVulkanQueue::GRAPHICS;
    case ERHIResourceState::PRESENT:
        return EVulkanQueue::PRESENT;
    default:
        return EVulkanQueue::ANY;
    }
}

VkPipelineStageFlags GetVulkanPipelineStageMask(ERHIResourceState State) {
    switch (State) {
    case ERHIResourceState::RENDER_TARGET:
        return VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    case ERHIResourceState::PRESENT:
        return VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    default:
        return VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    }
}

VkIndexType GetVulkanIndexType(ERHIFormat Format) {
    switch (Format) {
    case ERHIFormat::UNDEFINED:
    default:
        SK_LOG_ERROR("Invalid format for index buffer");
        return (VkIndexType) 0;
    }
}

VkDescriptorType GetVulkanDescriptorType(ERHIDescriptorType Type) {
    switch (Type) {
    case ERHIDescriptorType::UNIFORM_BUFFER:
        return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    case ERHIDescriptorType::TEXTURE:
        return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    default:
        SK_LOG_ERROR("Invalid descriptor type");
        return static_cast<VkDescriptorType>(0);
    }
}

VkShaderStageFlags GetVulkanShaderStageFlags(std::vector<ERHIShaderType> Types) {
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

VkShaderStageFlagBits GetVulkanShaderStage(ERHIShaderType Type) {
    switch (Type) {
    case ERHIShaderType::VERTEX:
        return VK_SHADER_STAGE_VERTEX_BIT;
    case ERHIShaderType::FRAGMENT:
        return VK_SHADER_STAGE_FRAGMENT_BIT;
    default:
        SK_LOG_ERROR("Invalid shader stage");
        return static_cast<VkShaderStageFlagBits>(0);
    }
}

VkVertexInputRate GetVulkanVertexInputRate(ERHIVertexInputRate InputRate) {
    switch (InputRate) {
    case ERHIVertexInputRate::PER_VERTEX:
        return VK_VERTEX_INPUT_RATE_VERTEX;
    case ERHIVertexInputRate::PER_INSTANCE:
        return VK_VERTEX_INPUT_RATE_INSTANCE;
    default:
        SK_LOG_ERROR("Invalid vertex input rate");
        return static_cast<VkVertexInputRate>(0);
    }
}

VkFormat GetVulkanFormat(ERHIFormat Format) {
    switch (Format) {
    case ERHIFormat::B8G8R8A8_SRGB:
        return VK_FORMAT_B8G8R8A8_SRGB;
    default:
        return VK_FORMAT_UNDEFINED;
    }
}

VkFormat GetVulkanDepthFormat(ERHIFormat Format) {
    switch (Format) {
    case ERHIFormat::UNDEFINED:
        return VK_FORMAT_UNDEFINED;
    default:
        return static_cast<VkFormat>(0);
    }
}

ERHIFormat GetRHIFormat(VkFormat Format) {
    switch (Format) {
    case VK_FORMAT_B8G8R8A8_SRGB:
        return ERHIFormat::B8G8R8A8_SRGB;
    default:
        SK_LOG_ERROR("Invalid vulkan format");
        return ERHIFormat::UNDEFINED;
    }
}

VkFormat GetVulkanStencilFormat(ERHIFormat Format) {
    switch (Format) {
    case ERHIFormat::UNDEFINED:
        return VK_FORMAT_UNDEFINED;
    default:
        SK_LOG_ERROR("Invalid stencil format");
        return static_cast<VkFormat>(0);
    }
}

FSInt32 FindPresentQueueIndex(VkPhysicalDevice GPU, VkSurfaceKHR Surface) {
    FUInt32 QueueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(GPU, &QueueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> QueueFamilies(QueueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(GPU, &QueueFamilyCount, QueueFamilies.data());

    for (FUInt32 QueueFamilyIndex = 0; QueueFamilyIndex < QueueFamilyCount; QueueFamilyIndex++) {
        VkBool32 PresentSupport = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(GPU, QueueFamilyIndex, Surface, &PresentSupport);
        if (PresentSupport) {
            return QueueFamilyIndex;
        }
    }

    return -1;
}

FVulkanRHI::FVulkanRHI() {
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
    }

    VkApplicationInfo AppInfo = {};
    AppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    AppInfo.pApplicationName = AppGetName();
    AppInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    AppInfo.pEngineName = "StakEngine";
    AppInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    AppInfo.apiVersion = VK_API_VERSION_1_3;

    VkInstanceCreateInfo InstanceInfo = {};
    InstanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    InstanceInfo.pApplicationInfo = &AppInfo;

    VkDebugUtilsMessengerCreateInfoEXT DebugMessengerInfo = {};

    if (ENABLE_VALIDATION_LAYERS) {
        InstanceInfo.enabledLayerCount = static_cast<FUInt32>(VALIDATION_LAYERS.size());
        InstanceInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();

        DebugMessengerInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        DebugMessengerInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        DebugMessengerInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        DebugMessengerInfo.pfnUserCallback = VkMessenger;

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

    InstanceInfo.enabledExtensionCount = static_cast<FUInt32>(Extensions.size());
    InstanceInfo.ppEnabledExtensionNames = Extensions.data();

    CHECK_VK_ERR(vkCreateInstance(&InstanceInfo, nullptr, &Instance), "Failed to create vulkan instance");
    CHECK_VK_ERR(CreateDebugMessenger(Instance, &DebugMessengerInfo, nullptr, &DebugMessenger), "Failed to create vulkan debug messenger");

    FUInt32 DeviceCount = 0;
    vkEnumeratePhysicalDevices(Instance, &DeviceCount, nullptr);
    std::vector<VkPhysicalDevice> Devices(DeviceCount);
    vkEnumeratePhysicalDevices(Instance, &DeviceCount, Devices.data());

    FBool DeviceFound = false;
    for (FUInt32 DeviceIndex = 0; DeviceIndex < DeviceCount; DeviceIndex++) {
        FBool IsDeviceSuitable = false;
        GraphicsQueueIndex = FindGraphicsQueueFamiliy(Devices[DeviceIndex]);

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

        IsDeviceSuitable = ExtensionsSupported && GraphicsQueueIndex >= 0 && SupportedFeatures.samplerAnisotropy;

        if (IsDeviceSuitable) {
            GPU = Devices[DeviceIndex];
            DeviceFound = true;
            break;
        }
    }

    if (!DeviceFound) {
        SK_LOG_ERROR("Failed to find suitable physical device");
    }

    FFloat QueuePriority = 1.0f;
    VkDeviceQueueCreateInfo QueueInfo = {};
    QueueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    QueueInfo.queueFamilyIndex = GraphicsQueueIndex;
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
    DeviceInfo.enabledExtensionCount = static_cast<FUInt32>(DEVICE_EXTENSIONS.size());
    DeviceInfo.ppEnabledExtensionNames = DEVICE_EXTENSIONS.data();

    if (ENABLE_VALIDATION_LAYERS) {
        DeviceInfo.enabledLayerCount = static_cast<FUInt32>(VALIDATION_LAYERS.size());
        DeviceInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();
    }

    CHECK_VK_ERR(vkCreateDevice(GPU, &DeviceInfo, nullptr, &Device), "Failed to create vulkan device");

    vkGetDeviceQueue(Device, GraphicsQueueIndex, 0, &GraphicsQueue);

    VkFenceCreateInfo FenceInfo = {};
    FenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    FenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (auto &Fence : InFlightFences) {
        CHECK_VK_ERR(vkCreateFence(Device, &FenceInfo, nullptr, &Fence), "Failed to create vulkan fence");
    }

    VkCommandPoolCreateInfo CommandPoolInfo = {};

    CommandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    CommandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    CommandPoolInfo.queueFamilyIndex = GraphicsQueueIndex;

    CHECK_VK_ERR(vkCreateCommandPool(Device, &CommandPoolInfo, nullptr, &CommandPool), "Failed to create vulkan command pool");
}

FVulkanRHI::~FVulkanRHI() {
    DestroyDebugMessenger(Instance, DebugMessenger, nullptr);

    vkDestroyInstance(Instance, nullptr);

    for (VkFence Fence : InFlightFences) {
        vkDestroyFence(Device, Fence, nullptr);
    }

    vkDestroyCommandPool(Device, CommandPool, nullptr);

    vkDestroyDevice(Device, nullptr);
}

void FVulkanRHI::ImGuiNewFrame() {
    ImGui_ImplVulkan_NewFrame();
}

void FVulkanRHI::ShutdownImGui() {
    ImGui_ImplVulkan_Shutdown();
    vkDestroyDescriptorPool(Device, ImGuiPool, nullptr);
}

void FVulkanRHI::InitImGui() {
    VkDescriptorPoolSize PoolSizes[] = {
        { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
    };

    VkDescriptorPoolCreateInfo PoolInfo = {};
    PoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    PoolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    PoolInfo.maxSets = 1000;
    PoolInfo.poolSizeCount = static_cast<FUInt32>(std::size(PoolSizes));
    PoolInfo.pPoolSizes = PoolSizes;

    CHECK_VK_ERR(vkCreateDescriptorPool(Device, &PoolInfo, nullptr, &ImGuiPool), "Failed to create imgui descriptor pool");

    ImGui_ImplVulkan_InitInfo InitInfo = {};
    InitInfo.Allocator = nullptr;
    InitInfo.CheckVkResultFn = [](VkResult Err) {
        CHECK_VK_ERR(Err, "ImGui Vulkan Error");
    };
    InitInfo.DescriptorPool = ImGuiPool;
    InitInfo.Instance = Instance;
    InitInfo.Device = Device;
    InitInfo.ImageCount = std::static_pointer_cast<FVulkanViewport>(ActiveViewport)->GetMinImageCount();
    InitInfo.MinImageCount = std::static_pointer_cast<FVulkanViewport>(ActiveViewport)->GetImageCount();
    InitInfo.MinAllocationSize = 1024 * 1024;
    InitInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    InitInfo.PhysicalDevice = GPU;
    InitInfo.PipelineCache = nullptr;
    VkPipelineRenderingCreateInfo PipelineInfo = {};
    PipelineInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    PipelineInfo.viewMask = 0x01;
    PipelineInfo.colorAttachmentCount = 1;
    VkFormat Formats[] = { GetVulkanFormat(ActiveViewport->GetBackbuffer()->GetFormat()) };
    PipelineInfo.pColorAttachmentFormats = Formats;
    PipelineInfo.depthAttachmentFormat = VK_FORMAT_UNDEFINED;
    PipelineInfo.stencilAttachmentFormat = VK_FORMAT_UNDEFINED;
    InitInfo.PipelineRenderingCreateInfo = PipelineInfo;
    InitInfo.Queue = GraphicsQueue;
    InitInfo.QueueFamily = GraphicsQueueIndex;
    InitInfo.RenderPass = VK_NULL_HANDLE;
    InitInfo.Subpass = 0;
    InitInfo.UseDynamicRendering = true;

    ImGui_ImplVulkan_Init(&InitInfo);
}

void FVulkanRHI::WaitForGPUIdle() {
    vkDeviceWaitIdle(Device);
}

void FVulkanRHI::Submit(TRef<IRHICommandContext> Context) {
    auto VulkanContext = std::static_pointer_cast<FVulkanCommandContext>(Context);
    VkCommandBuffer CommandBuffer = VulkanContext->GetMainCommandBuffer();

    VkSubmitInfo SubmitInfo = {};
    SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    VkPipelineStageFlags WaitStages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
    };
    SubmitInfo.waitSemaphoreCount = 1;
    SubmitInfo.pWaitSemaphores = &ActiveViewport->ImageAvailableSemaphores[CurrentFrame];
    SubmitInfo.pWaitDstStageMask = WaitStages;
    SubmitInfo.signalSemaphoreCount = 1;
    SubmitInfo.pSignalSemaphores = &ActiveViewport->RenderFinishedSemaphores[CurrentFrame];
    SubmitInfo.commandBufferCount = 1;
    SubmitInfo.pCommandBuffers = &CommandBuffer;

    vkResetFences(Device, 1, &InFlightFences[CurrentFrame]);
    
    CHECK_VK_ERR(vkQueueSubmit(GraphicsQueue, 1, &SubmitInfo, InFlightFences[CurrentFrame]), "Failed to submit to graphics queue");
}

TRef<IRHICommandContext> FVulkanRHI::CreateCommandContext() {
    return TCreateRef<FVulkanCommandContext>(Device);
}

TRef<IRHIShader> FVulkanRHI::CreateShader(const FRHIShaderDescription &Description) {
    return TCreateRef<FVulkanShader>(Device, Description);
}

TRef<IRHIBuffer> FVulkanRHI::CreateBuffer(const FRHIBufferDescription &Description) {
    return TCreateRef<FVulkanBuffer>(Device, Description);
}

TRef<IRHIPipelineLayout> FVulkanRHI::CreatePipelineLayout(const FRHIPipelineLayoutDescription &Description) {
    return TCreateRef<FVulkanPipelineLayout>(Device, Description);
}

TRef<IRHIDescriptorSetLayout> FVulkanRHI::CreateDescriptorSetLayout(const FRHIDescriptorSetLayoutDescription &Description) {
    return TCreateRef<FVulkanDescriptorSetLayout>(Device, Description);
}

TRef<IRHIPipeline> FVulkanRHI::CreatePipeline(const FRHIGraphicsPipelineStateDescription &Description) {
    return TCreateRef<FVulkanPipeline>(Device, Description);
}

VkCommandBuffer FVulkanRHI::BeginOneTimeCommandBuffer() {
    VkCommandBufferAllocateInfo AllocInfo = {};
    AllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    AllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    AllocInfo.commandPool = CommandPool;
    AllocInfo.commandBufferCount = 1;

    VkCommandBuffer CommandBuffer;
    vkAllocateCommandBuffers(Device, &AllocInfo, &CommandBuffer);

    VkCommandBufferBeginInfo BeginInfo = {};
    BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    BeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(CommandBuffer, &BeginInfo);

    return CommandBuffer;
}

void FVulkanRHI::EndOneTimeCommandBuffer(VkCommandBuffer CommandBuffer) {
    vkEndCommandBuffer(CommandBuffer);

    VkSubmitInfo SubmitInfo = {};
    SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    SubmitInfo.commandBufferCount = 1;
    SubmitInfo.pCommandBuffers = &CommandBuffer;

    vkQueueSubmit(GraphicsQueue, 1, &SubmitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(GraphicsQueue);

    vkFreeCommandBuffers(Device, CommandPool, 1, &CommandBuffer);
}

void FVulkanRHI::CopyBuffer(VkBuffer Src, VkBuffer Dst, FUInt32 Size) {
    VkCommandBuffer CommandBuffer = BeginOneTimeCommandBuffer();

    VkBufferCopy CopyRegion = {};
    CopyRegion.srcOffset = 0;
    CopyRegion.dstOffset = 0;
    CopyRegion.size = Size;

    vkCmdCopyBuffer(CommandBuffer, Src, Dst, 1, &CopyRegion);

    EndOneTimeCommandBuffer(CommandBuffer);
}

void FVulkanRHI::CreateBuffer(FUInt32 Size, VkBufferUsageFlags Usage, VkMemoryPropertyFlags Properties, VkBuffer *Buffer, VkDeviceMemory *Memory) {
    VkBufferCreateInfo BufferInfo = {};
    BufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    BufferInfo.size = Size;
    BufferInfo.usage = Usage;
    BufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    CHECK_VK_ERR(vkCreateBuffer(Device, &BufferInfo, nullptr, Buffer), "Failed to create buffer");

    VkMemoryRequirements MemReqs = {};
    vkGetBufferMemoryRequirements(Device, *Buffer, &MemReqs);

    VkMemoryAllocateInfo AllocInfo = {};
    AllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    AllocInfo.allocationSize = MemReqs.size;
    AllocInfo.memoryTypeIndex = FindMemoryType(MemReqs.memoryTypeBits, Properties);

    CHECK_VK_ERR(vkAllocateMemory(Device, &AllocInfo, nullptr, Memory), "Failed to allocate memory");
    vkBindBufferMemory(Device, *Buffer, *Memory, 0);
}

FUInt32 FVulkanRHI::FindMemoryType(FUInt32 Filter, VkMemoryPropertyFlags Flags) {
    VkPhysicalDeviceMemoryProperties MemProps;
    vkGetPhysicalDeviceMemoryProperties(GPU, &MemProps);

    for (FUInt32 MemoryTypeIndex = 0; MemoryTypeIndex < MemProps.memoryTypeCount; MemoryTypeIndex++) {
        if ((Filter & (1 << MemoryTypeIndex)) && ((MemProps.memoryTypes[MemoryTypeIndex].propertyFlags & Flags) == Flags)) {
            return MemoryTypeIndex;
        }
    }

    SK_LOG_ERROR("Failed to get memory type");
    return 0;
}

FUInt32 FVulkanRHI::GetCurrentFrameIndex() {
    return CurrentFrame;
}

void FVulkanRHI::PrepareFrame() {
    if (ActiveViewport == nullptr) {
        SK_LOG_WARN("No viewport set for rendering");
        return;
    }

    ActiveViewport->PrepareFrame(CurrentFrame);
}

void FVulkanRHI::PresentFrame() {
    if (ActiveViewport == nullptr) {
        SK_LOG_WARN("No viewport set for rendering");
        return;
    }

    ActiveViewport->PresentFrame(CurrentFrame);

    CurrentFrame = (CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

    vkWaitForFences(Device, 1, &InFlightFences[CurrentFrame], VK_TRUE, UINT64_MAX);
}

void FVulkanRHI::SetActiveViewport(TRef<IRHIViewport> Viewport) {
    ActiveViewport = std::static_pointer_cast<FVulkanViewport>(Viewport);
}


TRef<IRHITexture> FVulkanRHI::GetCurrentBackbuffer() {
    return ActiveViewport->GetBackbuffer();
}

TRef<IRHIViewport> FVulkanRHI::CreateViewport(void *WindowHandle) {
    return TCreateRef<FVulkanViewport>(Instance, GPU, Device, WindowHandle);
}

FUInt32 FVulkanRHI::GetCurrentImageIndex() {
    return ActiveViewport->GetCurrentImageIndex();
}

FUInt32 FVulkanRHI::GetActivePresentQueueIndex() {
    return ActiveViewport->GetPresentQueueIndex();
}
