#pragma once

#include "IRHITexture.h"
#include <vulkan/vulkan.h>

namespace Stak {

class VulkanRHITexture : public IRHITexture {
public:
  VulkanRHITexture(VkDevice device, const RHITextureDescription &desc);
  ~VulkanRHITexture();

  TextureSizeData getSize() { return { mWidth, mHeight }; }
  void setSize(i32 x, i32 y);

  void setUsage(ERHITextureUsage usage);
  ERHITextureUsage getUsage() { return mUsage; }

private:
  i32 mWidth;
  i32 mHeight;
  ERHITextureUsage mUsage;
  ERHIShaderStage mShaderStage;
  ERHIFormat mDataFormat;
  ERHIAttachmentType mAttachmentType;
private:
  VkImage mImage;
};


} // namespace Stak