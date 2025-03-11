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

bool GetSwapchainSupport(VulkanSwapchainSupport *Out, VkPhysicalDevice Device, VkSurfaceKHR Surface) {
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

VkAccessFlags GetVulkanAccessMask(ERHIResourceState State) {
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

VkImageLayout GetVulkanImageLayout(ERHIResourceState State) {
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
        return static_cast<VkDescriptorType>(0);
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
        return static_cast<VkShaderStageFlagBits>(0);
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
        return static_cast<VkVertexInputRate>(0);
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
        return static_cast<VkFormat>(0);
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

FUInt32 VulkanFindMemoryType(VkPhysicalDevice GPU, FUInt32 Filter, VkMemoryPropertyFlags Flags) {
    VkPhysicalDeviceMemoryProperties MemProps = {};
    vkGetPhysicalDeviceMemoryProperties(GPU, &MemProps);

    for (FUInt32 Index = 0; Index < MemProps.memoryTypeCount; Index++) {
        if ((Filter & (1 << Index)) && ((MemProps.memoryTypes[Index].propertyFlags & Flags) == Flags)) {
            return Index;
        }
    }

    SK_LOG_ERROR("Failed to get memory type\n");
    return static_cast<FUInt32>(-1);
}

bool FVulkanRHI::Init() {
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
        return false;
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

    if (vkCreateDevice(GPU, &DeviceInfo, nullptr, &Device) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to create vulkan device");
        return false;
    }

    vkGetDeviceQueue(Device, GraphicsQueueIndex, 0, &GraphicsQueue);

    VkFenceCreateInfo FenceInfo = {};
    FenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    FenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (auto &Fence : InFlightFences) {
        if (vkCreateFence(Device, &FenceInfo, nullptr, &Fence) != VK_SUCCESS) {
            SK_LOG_ERROR("Failed to create vulkan fence");
            return false;
        }
    }

    VkCommandPoolCreateInfo CommandPoolInfo = {};

    CommandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    CommandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    CommandPoolInfo.queueFamilyIndex = GraphicsQueueIndex;

    if (vkCreateCommandPool(Device, &CommandPoolInfo, nullptr, &CommandPool) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to create vulkan command pool");
        return false;
    }

    return true;
}

void FVulkanRHI::Shutdown() {
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
    VkFormat Formats[] = { VulkanGetFormat(ActiveViewport->GetCurrentBackbuffer()->GetFormat()) };

    VkPipelineRenderingCreateInfo PipelineInfo = {};
    PipelineInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    PipelineInfo.viewMask = 0x01;
    PipelineInfo.colorAttachmentCount = 1;
    PipelineInfo.pColorAttachmentFormats = Formats;
    PipelineInfo.depthAttachmentFormat = VK_FORMAT_UNDEFINED;
    PipelineInfo.stencilAttachmentFormat = VK_FORMAT_UNDEFINED;

    ImGui_ImplVulkan_InitInfo InitInfo = {};
    InitInfo.ApiVersion = VK_API_VERSION_1_3;
    InitInfo.Instance = Instance;
    InitInfo.PhysicalDevice = GPU;
    InitInfo.Device = Device;
    InitInfo.QueueFamily = GraphicsQueueIndex;
    InitInfo.Queue = GraphicsQueue;
    InitInfo.MinImageCount = std::static_pointer_cast<FVulkanViewport>(ActiveViewport)->GetMinImageCount();
    InitInfo.ImageCount = std::static_pointer_cast<FVulkanViewport>(ActiveViewport)->GetImageCount();
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

bool FVulkanRHI::WaitForGPUIdle() {
    if (vkDeviceWaitIdle(Device) != VK_SUCCESS) return false;
    return true;
}

bool FVulkanRHI::Submit(TRef<IRHICommandContext> Context) {
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

    if (vkResetFences(Device, 1, &InFlightFences[CurrentFrame]) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to reset fencecs");
        return false;
    }

    if (vkQueueSubmit(GraphicsQueue, 1, &SubmitInfo, InFlightFences[CurrentFrame]) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to submit to graphics queue");
        return false;
    }

    return true;
}

TRef<IRHICommandContext> FVulkanRHI::CreateCommandContext() {
    return TCreateRef<FVulkanCommandContext>(Device);
}

TRef<IRHIShader> FVulkanRHI::CreateShader() {
    return TCreateRef<FVulkanShader>(Device);
}

TRef<IRHIBuffer> FVulkanRHI::CreateBuffer() {
    return TCreateRef<FVulkanBuffer>(Device);
}

TRef<IRHIPipelineLayout> FVulkanRHI::CreatePipelineLayout() {
    return TCreateRef<FVulkanPipelineLayout>(Device);
}

TRef<IRHIDescriptorSetLayout> FVulkanRHI::CreateDescriptorSetLayout() {
    return TCreateRef<FVulkanDescriptorSetLayout>(Device);
}

TRef<IRHIPipeline> FVulkanRHI::CreatePipeline() {
    return TCreateRef<FVulkanPipeline>(Device);
}

VkCommandBuffer FVulkanRHI::BeginOneTimeCommandBuffer() {
    VkCommandBufferAllocateInfo AllocInfo = {};
    AllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    AllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    AllocInfo.commandPool = CommandPool;
    AllocInfo.commandBufferCount = 1;

    VkCommandBuffer CommandBuffer;
    if (vkAllocateCommandBuffers(Device, &AllocInfo, &CommandBuffer) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to allocate one time command buffer");
        return nullptr;
    }

    VkCommandBufferBeginInfo BeginInfo = {};
    BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    BeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    if (vkBeginCommandBuffer(CommandBuffer, &BeginInfo) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to begin one time command buffer");
        vkFreeCommandBuffers(Device, CommandPool, 1, &CommandBuffer);
        return nullptr;
    }

    return CommandBuffer;
}

bool FVulkanRHI::EndOneTimeCommandBuffer(VkCommandBuffer CommandBuffer) {
    if (CommandBuffer == nullptr) {
        SK_LOG_ERROR("Invalid command buffer provided");
        return false;
    }

    vkEndCommandBuffer(CommandBuffer);

    VkSubmitInfo SubmitInfo = {};
    SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    SubmitInfo.commandBufferCount = 1;
    SubmitInfo.pCommandBuffers = &CommandBuffer;

    if (vkQueueSubmit(GraphicsQueue, 1, &SubmitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to submit one time command buffer");
        vkFreeCommandBuffers(Device, CommandPool, 1, &CommandBuffer);
        return false;
    }
    if (vkQueueWaitIdle(GraphicsQueue) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to wait for queue");
        vkFreeCommandBuffers(Device, CommandPool, 1, &CommandBuffer);
        return false;
    }

    vkFreeCommandBuffers(Device, CommandPool, 1, &CommandBuffer);

    return true;
}

bool FVulkanRHI::CopyBuffer(VkBuffer Src, VkBuffer Dst, FUInt32 Size) {
    VkCommandBuffer CommandBuffer = BeginOneTimeCommandBuffer();

    if (CommandBuffer == nullptr) {
        SK_LOG_ERROR("Failed to get command buffer for buffer copy");
        return false;
    }

    VkBufferCopy CopyRegion = {};
    CopyRegion.srcOffset = 0;
    CopyRegion.dstOffset = 0;
    CopyRegion.size = Size;

    vkCmdCopyBuffer(CommandBuffer, Src, Dst, 1, &CopyRegion);

    if (!EndOneTimeCommandBuffer(CommandBuffer)) {
        SK_LOG_ERROR("Failed to submit command buffer for buffer copy");
        return false;
    }

    return true;
}

bool FVulkanRHI::CreateBuffer(FUInt32 Size, VkBufferUsageFlags Usage, VkMemoryPropertyFlags Properties, VkBuffer *Buffer, VkDeviceMemory *Memory) {
    VkBufferCreateInfo BufferInfo = {};
    BufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    BufferInfo.size = Size;
    BufferInfo.usage = Usage;
    BufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(Device, &BufferInfo, nullptr, Buffer) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to create buffer");
        return false;
    }

    VkMemoryRequirements MemReqs = {};
    vkGetBufferMemoryRequirements(Device, *Buffer, &MemReqs);

    VkMemoryAllocateInfo AllocInfo = {};
    AllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    AllocInfo.allocationSize = MemReqs.size;
    AllocInfo.memoryTypeIndex = FindMemoryType(MemReqs.memoryTypeBits, Properties);

    if (vkAllocateMemory(Device, &AllocInfo, nullptr, Memory) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to allocate buffer memory");
        return false;
    }

    if (vkBindBufferMemory(Device, *Buffer, *Memory, 0) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to bind buffer memory");
        return false;
    }

    return true;
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

bool FVulkanRHI::PrepareFrame() {
    if (ActiveViewport == nullptr) {
        SK_LOG_WARN("No viewport set for rendering");
        return false;
    }

    if (!ActiveViewport->PrepareFrame(CurrentFrame)) {
        SK_LOG_ERROR("Viewport failed to prepare frame");
        return false;
    }

    return true;
}

bool FVulkanRHI::PresentFrame() {
    if (ActiveViewport == nullptr) {
        SK_LOG_WARN("No viewport set for rendering");
        return false;
    }

    if (!ActiveViewport->PresentFrame(CurrentFrame)) {
        SK_LOG_ERROR("Viewport failed to present frame");
        return false;
    }

    CurrentFrame = (CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

    if (vkWaitForFences(Device, 1, &InFlightFences[CurrentFrame], VK_TRUE, UINT64_MAX) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to wait for fences");
        return false;
    }

    return true;
}

bool FVulkanRHI::SetActiveViewport(TRef<IRHIViewport> Viewport) {
    ActiveViewport = std::static_pointer_cast<FVulkanViewport>(Viewport);
    return true;
}

TRef<IRHITexture> FVulkanRHI::GetCurrentBackbuffer() {
    return ActiveViewport->GetCurrentBackbuffer();
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

TRef<IRHITexture> FVulkanRHI::CreateTexture() {
    return TCreateRef<FVulkanTexture>(Device, GPU);
}
