#pragma once

#include "IRHIShader.h"
#include <vulkan/vulkan.h>

namespace Stak {

class VulkanRHIShader : public IRHIShader {
public:
  VulkanRHIShader(VkDevice device, const RHIShaderDescription &desc);
  ~VulkanRHIShader();
};

} // namespace Stak