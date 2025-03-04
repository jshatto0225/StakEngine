#include "VulkanTexture.h"

#include "VulkanDevice.h"

FVulkanTexture::FVulkanTexture(FVulkanDevice &Device, VkSwapchainKHR Swapchain, FUInt32 ImageCount, VkExtent2D Extent, VkFormat Format) : Device(Device), Format(Format), Extent(Extent), Backbuffer(true) {
    RHIFormat = GetRHIFormat(Format);
    
    Images.resize(ImageCount);

    vkGetSwapchainImagesKHR(Device.GetVulkanDevice(), Swapchain, &ImageCount, Images.data());

    ImageViews.resize(Images.size());

    for (FUInt32 ImageViewIndex = 0; ImageViewIndex < Images.size(); ImageViewIndex++) {
        ImageViews[ImageViewIndex] = VulkanCreateImageView(Device.GetVulkanDevice(), Images[ImageViewIndex], Format);
    }

    SubresourceRanges.emplace_back(VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1);
}

FVulkanTexture::~FVulkanTexture() {
    if (Initialized) {
        Shutdown();
    }
}

void FVulkanTexture::Shutdown() {
    for (FUInt32 ImageIndex = 0; ImageIndex < Images.size(); ImageIndex++) {
        vkDestroyImageView(Device.GetVulkanDevice(), ImageViews[ImageIndex], nullptr);
    }

    Initialized = false;
}

VkImage FVulkanTexture::GetVulkanImage(FUInt32 ImageIndex) {
    if (ImageIndex > Images.size()) {
        SK_LOG_ERROR("Image index out of range");
        return VK_NULL_HANDLE;
    }
    return Images[ImageIndex];
}

VkImageView FVulkanTexture::GetVulkanImageView(FUInt32 ImageIndex) {
    if (ImageIndex > ImageViews.size()) {
        SK_LOG_ERROR("Image index out of range");
        return VK_NULL_HANDLE;
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
