#pragma once

#include "IRHIPipeline.h"

#include <vulkan/vulkan.h>

class FVulkanRHIPipeline final : public IRHIPipeline {
public:
  FVulkanRHIPipeline(TRef<IRHIDevice> Device);
  ~FVulkanRHIPipeline();


public:
  VkPipeline GetVkPipeline() const { return mPipeline; }

private:
  VkPipeline mPipeline = VK_NULL_HANDLE;
};
