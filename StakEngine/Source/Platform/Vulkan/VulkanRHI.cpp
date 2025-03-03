#include "VulkanRHI.h"

#include "Log.h"
#include "Application.h"
#include "VulkanCommandContext.h"
#include "VulkanDevice.h"

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
    auto Func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(Instance, "vkCreateDebugUtilsMessengerEXT");
    if (Func != nullptr) {
        return Func(Instance, Info, Allocator, Messenger);
    }
    else {
        SK_LOG_ERROR("Extension not present");
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void DestroyDebugMessenger(VkInstance Instance, VkDebugUtilsMessengerEXT Messenger, const VkAllocationCallbacks *Allocator) {
    auto Func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(Instance, "vkDestroyDebugUtilsMessengerEXT");
    if (Func != nullptr) {
        Func(Instance, Messenger, Allocator);
    }
}

VulkanQueueFamilies FindQueueFamilies(VkPhysicalDevice Device, VkSurfaceKHR Surface) {
    VulkanQueueFamilies Indices;

    FUInt32 QueueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(Device, &QueueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> QueueFamilies(QueueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(Device, &QueueFamilyCount, QueueFamilies.data());

    for (FUInt32 QueueFamilyIndex = 0; QueueFamilyIndex < QueueFamilyCount; QueueFamilyIndex++) {
        if (QueueFamilies[QueueFamilyIndex].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            Indices.GraphicsFamily = QueueFamilyIndex;
        }

        VkBool32 PresentSupport = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(Device, QueueFamilyIndex, Surface, &PresentSupport);
        if (PresentSupport) {
            Indices.PresentFamily = QueueFamilyIndex;
        }
        if (Indices.PresentFamily.has_value() && Indices.GraphicsFamily.has_value()) {
            break;
        }
    }

    return Indices;
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

FVulkanRHI::FVulkanRHI() {
    // INSTANCE
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

#ifdef SK_GLFW
    FUInt32 GlfwExtensionCount = 0;
    const char **GlfwExtensions = nullptr;
    GlfwExtensions = glfwGetRequiredInstanceExtensions(&GlfwExtensionCount);

    std::vector<const char *> Extensions(GlfwExtensions, GlfwExtensions + GlfwExtensionCount);
#else
    std::vector<const char *> extensions;
#endif

    if (ENABLE_VALIDATION_LAYERS) {
        Extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    InstanceInfo.enabledExtensionCount = static_cast<FUInt32>(Extensions.size());
    InstanceInfo.ppEnabledExtensionNames = Extensions.data();

    CHECK_VK_ERR(vkCreateInstance(&InstanceInfo, nullptr, &Instance), "Failed to create vulkan instance");
    CHECK_VK_ERR(CreateDebugMessenger(Instance, &DebugMessengerInfo, nullptr, &DebugMessenger), "Failed to create vulkan debug messenger");
}

FVulkanRHI::~FVulkanRHI() {
    DestroyDebugMessenger(Instance, DebugMessenger, nullptr);

    vkDestroyInstance(Instance, nullptr);
}

TRef<IRHIDevice> FVulkanRHI::CreateDevice(TRef<IWindow> Window) {
    return TCreateRef<FVulkanDevice>(Window, Instance);
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
    case ERHIFormat::UNDEFINED:
        return VK_FORMAT_UNDEFINED;
    default:
        SK_LOG_ERROR("Invalid format");
        return static_cast<VkFormat>(0);
    }
}

VkFormat GetVulkanDepthFormat(ERHIFormat Format) {
    switch (Format) {
    case ERHIFormat::UNDEFINED:
        return VK_FORMAT_UNDEFINED;
    default:
        SK_LOG_ERROR("Invalid depth format");
        return static_cast<VkFormat>(0);
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
