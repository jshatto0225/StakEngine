#include "FVulkanRHIResource.h"

FVulkanRHITexture::FVulkanRHITexture(TRef<FVulkanRHIDevice> Device, const FRHITextureDescription &Description) {
  mDescription = Description;
  mFormat = VK_FORMAT_UNDEFINED;
  mDevice = Device;

  if (Description.IsSwapchainImage) {
    GetSwapchainImages();
    return;
  }
}

FVulkanRHITexture::~FVulkanRHITexture() {
  if (mDescription.IsSwapchainImage) {
    return;
  }
}

void FVulkanRHITexture::GetSwapchainImages() {
  mImages = mDevice->GetVkSwapchainImages();
  mImageViews = mDevice->GetVkSwapchainImageViews();
  mFormat = mDevice->GetVkSwapchainImageFormat();
  mDescription.Width = mDevice->GetVkSwapchainWidth();
  mDescription.Height = mDevice->GetVkSwapchainHeight();
  mDescription.Layers = mDevice->GetVkSwapchainLayers();
}
