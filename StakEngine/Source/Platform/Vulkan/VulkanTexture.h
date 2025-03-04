#pragma once

#include "RHITexture.h"
#include "VulkanRHI.h"

class FVulkanDevice;

class FVulkanTexture : public IRHITexture {
public:
    FVulkanTexture(FVulkanDevice &Device, VkSwapchainKHR Swapchain, FUInt32 ImageCount, VkExtent2D Extent, VkFormat Format);
    ~FVulkanTexture();

    inline bool IsBackbuffer() override { return Backbuffer; }

    inline FRHIRenderArea GetRenderArea() override { return { 1, 0, 0, Extent.width, Extent.height }; }

    ERHIFormat GetFormat() override { return RHIFormat; }

    void Shutdown() override;

public:
    VkImage GetVulkanImage(FUInt32 ImageIndex);
    VkImageView GetVulkanImageView(FUInt32 ImageIndex);
    VkImageSubresourceRange GetVulkanSubresourceRange(FUInt32 Index);
    VkFormat GetVulkanFormat();
    FUInt32 GetImageCount();

private:
    bool Initialized = true;

    FVulkanDevice &Device;

    bool Backbuffer = false;

    std::vector<VkImage> Images;
    std::vector<VkImageView> ImageViews;
    std::vector<VkImageSubresourceRange> SubresourceRanges;
    VkFormat Format = VK_FORMAT_UNDEFINED;
    ERHIFormat RHIFormat = ERHIFormat::UNDEFINED;
    VkExtent2D Extent = {};
};
