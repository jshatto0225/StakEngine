#pragma once

#include "IRHIShader.h"
#include <vulkan/vulkan.h>

class VulkanRHIShader final : public IRHIShader {
public:
  VulkanRHIShader();
  ~VulkanRHIShader();
};