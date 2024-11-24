#include "FVulkanRHIResource.h"

FVulkanRHITexture::FVulkanRHITexture(TRef<FVulkanRHIDevice> Device, const FRHITextureDescription &Description) {
  mDevice = Device;

  if (Description.IsSwapchainImage) {
    GetSwapchainImages();
    return;
  }
}

FVulkanRHITexture::~FVulkanRHITexture() {
  if (mIsSwapchainImage) {
    return;
  }
}
bool IsSwapchainImage;

FUInt32 Width;
FUInt32 Height;
FUInt32 Layers;

ERHIStoreOp StoreOp;
ERHILoadOp LoadOp;

ERHIResourceUsage Usage;

FRHIClearValue ClearValue;
void FVulkanRHITexture::GetSwapchainImages() {
  mIsSwapchainImage = true;
  mDescriptions.reserve(mDevice->GetSwapchainImageCount());
  for (FUInt32 i = 0; i < mDevice->GetSwapchainImageCount(); i++) {
    mDescriptions.emplace_back(
      true, 
      mDevice->GetSwapchainWidth(), 
      mDevice->GetSwapchainHeight(),
      mDevice->GetSwapchainLayers(),
      ERHIStoreOp::STORE,
      ERHILoadOp::CLEAR,
      ERHIResourceUsage::RESOURCE_STATE_UNDEFINED
    );
  }

  mFormat = mDevice->GetVkSwapchainImageFormat();
}

VkImage FVulkanRHITexture::GetVkImage() const {
  if (mIsSwapchainImage) {
    return mDevice->GetCurrentVkSwapchainImage();
  }
  return mImages[mDevice->GetCurrentFrameIndex()];
}

VkImageView FVulkanRHITexture::GetVkImageView() const {
  if (mIsSwapchainImage) {
    return mDevice->GetCurrentVkSwapchainImageView();
  }
  return mImageViews[mDevice->GetCurrentFrameIndex()];
}

void FVulkanRHITexture::SetUsage(ERHIResourceUsage Usage) {
  mDescriptions[mDevice->GetCurrentSwapchainImageIndex()].Usage = Usage;
}

FUInt32 FVulkanRHITexture::GetWidth() const {
  if (mIsSwapchainImage) {
    return mDescriptions[mDevice->GetCurrentSwapchainImageIndex()].Width;
  }
}

FUInt32 FVulkanRHITexture::GetHeight() const {
  if (mIsSwapchainImage) {
    return mDescriptions[mDevice->GetCurrentSwapchainImageIndex()].Height;
  }
}

FUInt32 FVulkanRHITexture::GetLayers() const {
  if (mIsSwapchainImage) {
    return mDescriptions[mDevice->GetCurrentSwapchainImageIndex()].Layers;
  }
}

const FRHITextureDescription &FVulkanRHITexture::GetDescription() const {
  if (mIsSwapchainImage) {
    return mDescriptions[mDevice->GetCurrentSwapchainImageIndex()];
  }
}

ERHIResourceUsage FVulkanRHITexture::GetUsage() const {
  if (mIsSwapchainImage) {
    return mDescriptions[mDevice->GetCurrentSwapchainImageIndex()].Usage;
  }
}

