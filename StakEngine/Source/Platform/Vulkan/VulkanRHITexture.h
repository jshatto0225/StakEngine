#pragma once

#include "IRHITexture.h"
#include <vulkan/vulkan.h>

namespace Stak {

class VulkanRHITexture : public IRHITexture {
public:
  VulkanRHITexture(VkDevice device, TextureDescription &desc);
};

} // namespace Stak