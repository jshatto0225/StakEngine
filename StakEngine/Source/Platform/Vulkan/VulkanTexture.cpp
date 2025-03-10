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
        if (!CreateImage(Device, GPU, Extent.width, Extent.height, Format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &Images[ImageIndex], &ImageMemories[ImageIndex])) {
            SK_LOG_ERROR("Failed to create offscreen render target image");
            return false;
        }

        if (!CreateImageView(&ImageViews[ImageIndex], Device, Images[ImageIndex], Format)) {
            SK_LOG_ERROR("Failed to create image view for offscreen buffer");
            return false;
        }

        if (!CreateTextureSampler(&Samplers[ImageIndex], Device, GPU)) {
            SK_LOG_ERROR("Failed to create sampler for offscreen backbuffer");
            return false;
        }

        ImGuiDescriptorSets[ImageIndex] = ImGui_ImplVulkan_AddTexture(Samplers[ImageIndex], ImageViews[ImageIndex], VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }

    SubresourceRanges.emplace_back(VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1);

    return true;
}

bool FVulkanTexture::CreateImageView(VkImageView *Out, VkDevice Device, VkImage Image, VkFormat Format) {
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

    if (vkCreateImageView(Device, &ViewInfo, nullptr, Out) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to create image view");
        return false;
    }

    return true;
}


bool FVulkanTexture::CreateTextureSampler(VkSampler *OutSampler, VkDevice Device, VkPhysicalDevice GPU) {
    VkSamplerCreateInfo SamplerInfo = {};
    SamplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    SamplerInfo.magFilter = VK_FILTER_LINEAR;
    SamplerInfo.minFilter = VK_FILTER_LINEAR;
    SamplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    SamplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    SamplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    SamplerInfo.anisotropyEnable = VK_TRUE;

    VkPhysicalDeviceProperties Properties = { 0 };
    vkGetPhysicalDeviceProperties(GPU, &Properties);
    SamplerInfo.maxAnisotropy = Properties.limits.maxSamplerAnisotropy;

    SamplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    SamplerInfo.unnormalizedCoordinates = VK_FALSE;
    SamplerInfo.compareEnable = VK_FALSE;
    SamplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    SamplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    SamplerInfo.mipLodBias = 0.0f;
    SamplerInfo.minLod = 0.0f;
    SamplerInfo.maxLod = 0.0f;

    if (vkCreateSampler(Device, &SamplerInfo, nullptr, OutSampler) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to create image sampler");
        return false;
    }

    return true;
}

bool FVulkanTexture::CreateImage(VkDevice Device, VkPhysicalDevice GPU, FUInt32 Width, FUInt32 Height, VkFormat Format, VkImageTiling Tiling, VkImageUsageFlags Flags, VkMemoryPropertyFlags Properties, VkImage *OutImage, VkDeviceMemory *OutImageMemory) {
    VkImageCreateInfo ImageInfo = {};
    ImageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    ImageInfo.imageType = VK_IMAGE_TYPE_2D;
    ImageInfo.extent.width = Width;
    ImageInfo.extent.height = Height;
    ImageInfo.extent.depth = 1;
    ImageInfo.mipLevels = 1;
    ImageInfo.arrayLayers = 1;
    ImageInfo.format = Format;
    ImageInfo.tiling = Tiling;
    ImageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    ImageInfo.usage = Flags;
    ImageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    ImageInfo.samples = VK_SAMPLE_COUNT_1_BIT;

    if (vkCreateImage(Device, &ImageInfo, nullptr, OutImage) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to create image");
        return false;
    }

    VkMemoryRequirements MemReqs = {};
    vkGetImageMemoryRequirements(Device, *OutImage, &MemReqs);

    VkMemoryAllocateInfo AllocInfo = {};
    AllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    AllocInfo.allocationSize = MemReqs.size;
    AllocInfo.memoryTypeIndex = VulkanFindMemoryType(GPU, MemReqs.memoryTypeBits, Properties);

    if (vkAllocateMemory(Device, &AllocInfo, nullptr, OutImageMemory) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to allocate image memory");
        return false;
    }

    vkBindImageMemory(Device, *OutImage, *OutImageMemory, 0);

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
        if (!CreateImageView(&ImageViews[ImageViewIndex], Device, Images[ImageViewIndex], Format)) {
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
        if (!Backbuffer) {
            vkFreeMemory(Device, ImageMemories[ImageIndex], nullptr);
            vkDestroyImage(Device, Images[ImageIndex], nullptr);
            vkDestroySampler(Device, Samplers[ImageIndex], nullptr);
            ImGui_ImplVulkan_RemoveTexture(ImGuiDescriptorSets[ImageIndex]);
        }
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