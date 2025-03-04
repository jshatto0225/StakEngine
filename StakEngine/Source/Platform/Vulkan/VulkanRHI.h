#pragma once

#include "RHI.h"
#include "Log.h"
#include "RHIResource.h"
#include "RHIDescriptorSetLayout.h"
#include "RHIShader.h"
#include "RHIPipeline.h"

#include <vulkan/vulkan.h>
#include <vector>
#include <optional>

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

VulkanQueueFamilies FindQueueFamilies(VkPhysicalDevice Device, VkSurfaceKHR Surface);

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

class FVulkanRHI : public FRHI {
public:
    FVulkanRHI();
    ~FVulkanRHI();

    TRef<IRHIDevice> CreateDevice(TRef<IWindow> Window) override;

private:
    VkInstance Instance = VK_NULL_HANDLE;
    VkDevice Device = VK_NULL_HANDLE;
    VkPhysicalDevice GPU = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT DebugMessenger = VK_NULL_HANDLE;
    VkDescriptorPool ImGuiPool = VK_NULL_HANDLE;
};
