#include "VulkanTexture.h"

#include <backends/imgui_impl_vulkan.h>

FVulkanTexture::FVulkanTexture(VkDevice Device, VkPhysicalDevice GPU) : Device(Device), GPU(GPU) {}

bool FVulkanTexture::Init(const FRHIOffscreenRenderTargetDescription &Description) {
    RHIFormat = Description.Format;
    Format = VulkanGetFormat(RHIFormat);
    Extent = { Description.Width, Description.Height };
    ImageCount = MAX_FRAMES_IN_FLIGHT;
    
    Images.resize(ImageCount);
    ImageViews.resize(ImageCount);
    Samplers.resize(ImageCount);
    ImageMemories.resize(ImageCount);
    ImGuiDescriptorSets.resize(ImageCount);

    for (FUInt32 ImageIndex = 0; ImageIndex < ImageCount; ImageIndex++) {
        if (!VulkanCreateImage(Device, GPU, Extent.width, Extent.height, Format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &Images[ImageIndex], &ImageMemories[ImageIndex])) {
            SK_LOG_ERROR("Failed to create offscreen render target image");
            return false;
        }

        if (!VulkanCreateImageView(&ImageViews[ImageIndex], Device, Images[ImageIndex], Format)) {
            SK_LOG_ERROR("Failed to create image view for offscreen buffer");
            return false;
        }

        if (!VulkanCreateTextureSampler(&Samplers[ImageIndex], Device, GPU)) {
            SK_LOG_ERROR("Failed to create sampler for offscreen backbuffer");
            return false;
        }

        ImGuiDescriptorSets[ImageIndex] = ImGui_ImplVulkan_AddTexture(Samplers[ImageIndex], ImageViews[ImageIndex], VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }

    SubresourceRanges.emplace_back(VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1);

    return true;
}

bool FVulkanTexture::Init(VkSwapchainKHR NewSwapchain, FUInt32 NewImageCount, VkExtent2D NewExtent, VkFormat NewFormat) {
    Backbuffer = true;
    Swapchain = NewSwapchain;
    ImageCount = NewImageCount;
    Extent = NewExtent;
    Format = NewFormat;

    RHIFormat = VulkanGetRHIFormat(Format);

    Images.resize(ImageCount);

    if (vkGetSwapchainImagesKHR(Device, Swapchain, &ImageCount, Images.data()) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to get swapchain image");
        return false;
    }

    ImageViews.resize(Images.size());

    for (FUInt32 ImageViewIndex = 0; ImageViewIndex < Images.size(); ImageViewIndex++) {
        if (!VulkanCreateImageView(&ImageViews[ImageViewIndex], Device, Images[ImageViewIndex], Format)) {
            SK_LOG_ERROR("Failed to create image view");
            return false;
        }
    }

    SubresourceRanges.emplace_back(VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1);

    return true;
}

// TODO: 
void FVulkanTexture::Shutdown() {
    for (FUInt32 ImageIndex = 0; ImageIndex < Images.size(); ImageIndex++) {
        vkDestroyImageView(Device, ImageViews[ImageIndex], nullptr);
    }
}

VkImage FVulkanTexture::GetVulkanImage(FUInt32 ImageIndex) {
    if (ImageIndex > Images.size()) {
        SK_LOG_ERROR("Image index out of range");
        return nullptr;
    }
    return Images[ImageIndex];
}

VkImageView FVulkanTexture::GetVulkanImageView(FUInt32 ImageIndex) {
    if (ImageIndex > ImageViews.size()) {
        SK_LOG_ERROR("Image index out of range");
        return nullptr;
    }
    return ImageViews[ImageIndex];
}

VkImageSubresourceRange FVulkanTexture::GetVulkanSubresourceRange(FUInt32 Index) {
    if (Index > SubresourceRanges.size()) {
        SK_LOG_ERROR("Subresource index out of range");
        return {};
    }
    return SubresourceRanges[Index];
}

VkFormat FVulkanTexture::GetVulkanFormat() {
    return Format;
}

FUInt32 FVulkanTexture::GetImageCount() {
    return ImageCount;
}

FUInt64 FVulkanTexture::GetImGuiImageHandle(FUInt32 CurrentFrame) {
    return reinterpret_cast<FUInt64>(ImGuiDescriptorSets[CurrentFrame]);
}