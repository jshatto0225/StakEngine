#pragma once

#include "RHITexture.h"
#include "VulkanRHI.h"

class FVulkanDevice;

class FVulkanTexture : public IRHITexture {
public:
    FVulkanTexture(VkDevice Device, VkPhysicalDevice GPU);

    inline bool IsSwapchainBackbuffer() override { return SwapchainBackbuffer; }

    inline FRHIRenderArea GetRenderArea() override { return { 1, 0, 0, Extent.width, Extent.height }; }

    ERHIFormat GetFormat() override { return RHIFormat; }

    bool Init(const FRHIOffscreenRenderTargetDescription &Description) override;
    void Shutdown() override;

    void AddToImGuiWindow() override;

public:
    bool Init(VkImage SwapchainImage, VkExtent2D SwapchainExtent, VkFormat SwapchainFormat);
    VkImage GetVulkanImage();
    VkImageView GetVulkanImageView();
    VkImageSubresourceRange GetVulkanSubresourceRange(FUInt32 Index);
    VkFormat GetVulkanFormat();

    FUInt64 GetImGuiImageHandle();

    bool CreateImageView(VkImageView *Out, VkDevice Device, VkImage Image, VkFormat Format);
    bool CreateImage(VkDevice Device, VkPhysicalDevice GPU, FUInt32 Width, FUInt32 Height, VkFormat Format, VkImageTiling Tiling, VkImageUsageFlags Flags, VkMemoryPropertyFlags Properties, VkImage *OutImage, VkDeviceMemory *OutImageMemory);
    bool CreateTextureSampler(VkSampler *Out, VkDevice Device, VkPhysicalDevice GPU);

private:
    VkDevice Device;
    VkPhysicalDevice GPU;

    bool SwapchainBackbuffer = false;
    VkImage Image;
    VkDeviceMemory ImageMemory;
    VkImageView ImageView;
    VkSampler Sampler;
    std::vector<VkImageSubresourceRange> SubresourceRanges;
    VkFormat Format = VK_FORMAT_UNDEFINED;
    ERHIFormat RHIFormat = ERHIFormat::UNDEFINED;
    VkExtent2D Extent = {};
    FUInt32 ImageCount = 0;
    VkDescriptorSet ImGuiDescriptorSet;
};
