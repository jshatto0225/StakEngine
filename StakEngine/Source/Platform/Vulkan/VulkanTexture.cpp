#include "VulkanTexture.h"

FVulkanTexture::FVulkanTexture(VkDevice Device) : Device(Device) {}

bool FVulkanTexture::Init() {
    SK_LOG_WARN("Non swapchain backbuffer textures are not implemented");
    return false;
}

bool FVulkanTexture::Init(VkSwapchainKHR NewSwapchain, FUInt32 NewImageCount, VkExtent2D NewExtent, VkFormat NewFormat) {
    Backbuffer = true;
    Swapchain = NewSwapchain;
    ImageCount = NewImageCount;
    Extent = NewExtent;
    Format = NewFormat;

    RHIFormat = GetRHIFormat(Format);

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
    return static_cast<FUInt32>(Images.size());
}
