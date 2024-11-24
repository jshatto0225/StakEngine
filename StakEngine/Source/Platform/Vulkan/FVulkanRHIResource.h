#pragma once

#include "IRHIResource.h"

#include "FVulkanRHIDevice.h"

class FVulkanRHIBuffer final : public IRHIBuffer {
public:
  FVulkanRHIBuffer();
  ~FVulkanRHIBuffer();

public:
  VkBuffer GetVkBuffer() const { return mBuffer; }

private:
  VkBuffer mBuffer = VK_NULL_HANDLE;
};

class FVulkanRHITexture final : public IRHITexture {
public:
  FVulkanRHITexture(TRef<FVulkanRHIDevice> Device, const FRHITextureDescription &Description);
  ~FVulkanRHITexture();

  inline ERHIResourceType GetType() const override { return ERHIResourceType::TEXTURE; }

  FUInt32 GetWidth() const override;
  FUInt32 GetHeight() const override;
  FUInt32 GetLayers() const override;

  inline FBool IsSwapchainImage() const override { return mIsSwapchainImage; }

  const FRHITextureDescription &GetDescription() const override;

  ERHIResourceUsage GetUsage() const override;

  void SetUsage(ERHIResourceUsage Usage) override;

public:
  VkImage GetVkImage() const;
  VkImageView GetVkImageView() const;
  inline VkFormat GetVkFormat() const { return mFormat; }

private:
  void GetSwapchainImages();

private:
  std::vector<FRHITextureDescription> mDescriptions;
  FBool mIsSwapchainImage = false;

private:
  TRef<FVulkanRHIDevice> mDevice;

private:
  std::vector<VkImage> mImages = {};
  std::vector<VkImageView> mImageViews = {};
  VkFormat mFormat = VK_FORMAT_UNDEFINED;
};