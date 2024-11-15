#pragma once

#include "IRHIResource.h"

#include "FVulkanRHIDevice.h"

class FVulkanRHIBuffer final : public IRHIBuffer {

};

class FVulkanRHITexture final : public IRHITexture {
public:
  FVulkanRHITexture(TRef<IRHIDevice> Device, FRHITextureDescription &Description);
  ~FVulkanRHITexture();

  inline FUInt32 GetWidth() { return mDescription.Width; }
  inline FUInt32 GetHeight() { return mDescription.Height; }
  inline FUInt32 GetLayers() { return mDescription.Layers; }

public:
  inline VkImage GetVkImage() const { return mImage; }
  inline VkImageView GetVkImageView() const { return mImageView; }
  inline VkFormat GetVkFormat() const { return mFormat; }

private:
  void GetSwapchainImage();

private:
  FRHITextureDescription mDescription;

private:
  TRef<FVulkanRHIDevice> mDevice;

private:
  VkImage mImage;
  VkImageView mImageView;
  VkFormat mFormat;
};