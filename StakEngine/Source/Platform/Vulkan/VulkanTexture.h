#pragma once

#include "RHITexture.h"
#include "VulkanRHI.h"

struct FVulkanTexture : public IRHITexture {
    FVulkanTexture(VkDevice Device, VkPhysicalDevice GPU);

    bool Init(FRHIOffscreenRenderTargetDescription *Description) override;
    void Shutdown() override;

    void AddToImGuiWindow() override;

    bool Init(VkImage SwapchainImage, VkExtent2D SwapchainExtent, VkFormat SwapchainFormat);

    bool CreateImageView(VkImageView *Out, VkDevice Device, VkImage Image, VkFormat Format);
    bool CreateImage(VkDevice Device, VkPhysicalDevice GPU, FUInt32 Width, FUInt32 Height, VkFormat Format, VkImageTiling Tiling, VkImageUsageFlags Flags, VkMemoryPropertyFlags Properties, VkImage *OutImage, VkDeviceMemory *OutImageMemory);
    bool CreateTextureSampler(VkSampler *Out, VkDevice Device, VkPhysicalDevice GPU);

    VkDevice Device;
    VkPhysicalDevice GPU;

    bool SwapchainBackbuffer = false;
    VkImage Image = nullptr;
    VkDeviceMemory ImageMemory = nullptr;
    VkImageView ImageView = nullptr;
    VkSampler Sampler = nullptr;
    std::vector<VkImageSubresourceRange> SubresourceRanges;
    VkFormat VulkanFormat = VK_FORMAT_UNDEFINED;
    VkExtent2D Extent = {};
    FUInt32 ImageCount = 0;
    VkDescriptorSet ImGuiDescriptorSet = nullptr;
};
