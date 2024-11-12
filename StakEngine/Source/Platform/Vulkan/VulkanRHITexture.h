#pragma once

#include "IRHITexture.h"
#include <vulkan/vulkan.h>

namespace Stak {

class VulkanRHITexture : public IRHITexture {
public:
  VulkanRHITexture(VkDevice device, RHITextureDescription &desc);

  TextureSizeData getSize() { return { mWidth, mHeight }; }
  void setSize(i32 x, i32 y);

private:
  i32 mWidth;
  i32 mHeight;
};

} // namespace Stak