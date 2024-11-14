#include "FVulkanRHIResource.h"

FVulkanRHITexture::FVulkanRHITexture(TRef<IRHIDevice> Device, FRHITextureDescription &Description) {
  mDescription = Description;
  mFormat = VK_FORMAT_UNDEFINED;
  mImage = VK_NULL_HANDLE;
  mImageView = VK_NULL_HANDLE;

  if (Description.IsSwapchainImage) {
    GetSwapchainImage();
    return;
  }
}

FVulkanRHITexture::~FVulkanRHITexture() {
  if (mDescription.IsSwapchainImage) {
    return;
  }
}

void FVulkanRHITexture::GetSwapchainImage() {
  mImage = mDevice->GetSwapchainImage(mDescription.SwapchainImageIndex);
  mImageView = mDevice->GetSwapchainImageView(mDescription.SwapchainImageIndex);
  mFormat = mDevice->GetSwapchainImageFormat();
}
