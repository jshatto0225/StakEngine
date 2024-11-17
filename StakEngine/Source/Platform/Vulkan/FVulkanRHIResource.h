#pragma once

#include "IRHIResource.h"

#include "FVulkanRHIDevice.h"

class FVulkanRHIBuffer final : public IRHIBuffer {

};

class FVulkanRHITexture final : public IRHITexture {
public:
  FVulkanRHITexture(TRef<FVulkanRHIDevice> Device, const FRHITextureDescription &Description);
  ~FVulkanRHITexture();

  inline FUInt32 GetWidth() { return mDescription.Width; }
  inline FUInt32 GetHeight() { return mDescription.Height; }
  inline FUInt32 GetLayers() { return mDescription.Layers; }

public:
  inline const std::vector<VkImage> &GetVkImages() const { return mImages; }
  inline const std::vector<VkImageView> &GetVkImageViews() const { return mImageViews; }
  inline VkFormat GetVkFormat() const { return mFormat; }

private:
  void GetSwapchainImages();

private:
  FRHITextureDescription mDescription;

private:
  TRef<FVulkanRHIDevice> mDevice;

private:
  std::vector<VkImage> mImages;
  std::vector<VkImageView> mImageViews;
  VkFormat mFormat;
};