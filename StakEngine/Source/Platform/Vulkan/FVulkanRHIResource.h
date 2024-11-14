#pragma once

#include "IRHIResource.h"

#include "FVulkanRHIDevice.h"

class FVulkanRHIBuffer final : public IRHIBuffer {

};

class FVulkanRHITexture final : public IRHITexture {
public:
  FVulkanRHITexture(TRef<IRHIDevice> Device, FRHITextureDescription &Description);
  ~FVulkanRHITexture();

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