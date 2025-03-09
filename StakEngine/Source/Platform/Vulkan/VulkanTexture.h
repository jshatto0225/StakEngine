#pragma once

#include "RHITexture.h"
#include "VulkanRHI.h"

class FVulkanDevice;

class FVulkanTexture : public IRHITexture {
public:
    FVulkanTexture(VkDevice Device, VkPhysicalDevice GPU);

    inline bool IsBackbuffer() override { return Backbuffer; }

    inline FRHIRenderArea GetRenderArea() override { return { 1, 0, 0, Extent.width, Extent.height }; }

    ERHIFormat GetFormat() override { return RHIFormat; }

    bool Init(const FRHIOffscreenRenderTargetDescription &Description) override;
    void Shutdown() override;

public:
    bool Init(VkSwapchainKHR NewSwapchain, FUInt32 NewImageCount, VkExtent2D NewExtent, VkFormat NewFormat); // NOTE: For swapchain backbuffers
    VkImage GetVulkanImage(FUInt32 ImageIndex);
    VkImageView GetVulkanImageView(FUInt32 ImageIndex);
    VkImageSubresourceRange GetVulkanSubresourceRange(FUInt32 Index);
    VkFormat GetVulkanFormat();
    FUInt32 GetImageCount();

    FUInt64 GetImGuiImageHandle(FUInt32 CurrentFrame);

private:
    VkDevice Device;
    VkPhysicalDevice GPU;

    bool Backbuffer = false;
    std::vector<VkImage> Images;
    std::vector<VkDeviceMemory> ImageMemories;
    std::vector<VkImageView> ImageViews;
    std::vector<VkSampler> Samplers;
    std::vector<VkImageSubresourceRange> SubresourceRanges;
    VkFormat Format = VK_FORMAT_UNDEFINED;
    ERHIFormat RHIFormat = ERHIFormat::UNDEFINED;
    VkExtent2D Extent = {};
    FUInt32 ImageCount = 0;
    VkSwapchainKHR Swapchain = nullptr; // NOTE: For swapchain backbuffers
    std::vector<VkDescriptorSet> ImGuiDescriptorSets;
};
