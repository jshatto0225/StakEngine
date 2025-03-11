#include "VulkanTexture.h"

#include <backends/imgui_impl_vulkan.h>

FVulkanTexture::FVulkanTexture(VkDevice Device, VkPhysicalDevice GPU) : Device(Device), GPU(GPU) {}

bool FVulkanTexture::Init(FRHIOffscreenRenderTargetDescription *Description) {
    RHIFormat = Description->Format;
    Format = VulkanGetFormat(RHIFormat);
    Extent = { Description->Width, Description->Height };

    if (!CreateImage(Device, GPU, Extent.width, Extent.height, Format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &Image, &ImageMemory)) {
        SK_LOG_ERROR("Failed to create offscreen render target image");
        return false;
    }

    if (!CreateImageView(&ImageView, Device, Image, Format)) {
        SK_LOG_ERROR("Failed to create image view for offscreen buffer");
        return false;
    }

    if (!CreateTextureSampler(&Sampler, Device, GPU)) {
        SK_LOG_ERROR("Failed to create sampler for offscreen backbuffer");
        return false;
    }

    ImGuiDescriptorSet = ImGui_ImplVulkan_AddTexture(Sampler, ImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

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

bool FVulkanTexture::Init(VkImage SwapchainImage, VkExtent2D SwapchainExtent, VkFormat SwapchainFormat) {
    SwapchainBackbuffer = true;
    Extent = SwapchainExtent;
    Format = SwapchainFormat;
    RHIFormat = VulkanGetRHIFormat(Format);
    Image = SwapchainImage;

    if (!CreateImageView(&ImageView, Device, Image, Format)) {
        SK_LOG_ERROR("Failed to create image view");
        return false;
    }

    SubresourceRanges.emplace_back(VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1);

    return true;
}

void FVulkanTexture::Shutdown() {
    if (!SwapchainBackbuffer) {
        ImGui_ImplVulkan_RemoveTexture(ImGuiDescriptorSet);
    }

    vkDestroyImageView(Device, ImageView, nullptr);

    if (!SwapchainBackbuffer) {
        vkFreeMemory(Device, ImageMemory, nullptr);
        vkDestroyImage(Device, Image, nullptr);
        vkDestroySampler(Device, Sampler, nullptr);
    }
}

VkImage FVulkanTexture::GetVulkanImage() {
    return Image;
}

VkImageView FVulkanTexture::GetVulkanImageView() {
    return ImageView;
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

FUInt64 FVulkanTexture::GetImGuiImageHandle() {
    return reinterpret_cast<FUInt64>(ImGuiDescriptorSet);
}

void FVulkanTexture::AddToImGuiWindow() {
    ImVec2 ViewportPanelSize = ImGui::GetContentRegionAvail();

    // Keep aspect ratio
    FFloat Aspect = static_cast<FFloat>(Extent.width) / static_cast<FFloat>(Extent.height);
    ImVec2 ImageSize;
    if (ViewportPanelSize.x / Aspect <= ViewportPanelSize.y) {
        ImageSize.x = ViewportPanelSize.x;
        ImageSize.y = ViewportPanelSize.x / Aspect;
    } else {
        ImageSize.y = ViewportPanelSize.y;
        ImageSize.x = ViewportPanelSize.y * Aspect;
    }

    // Center Image
    ImVec2 Offset = { (ViewportPanelSize.x - ImageSize.x) * 0.5f, (ViewportPanelSize.y - ImageSize.y) * 0.5f };
    Offset.x = (Offset.x > 0) ? Offset.x : 0;
    Offset.y = (Offset.y > 0) ? Offset.y : 0;
    ImVec2 CursorPos = ImGui::GetCursorPos();
    CursorPos.x += Offset.x;
    CursorPos.y += Offset.y;
    ImGui::SetCursorPos(CursorPos);

    ImGui::Image(reinterpret_cast<FUInt64>(ImGuiDescriptorSet), ImageSize);
}