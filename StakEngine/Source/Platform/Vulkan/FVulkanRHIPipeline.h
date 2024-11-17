#pragma once

#include "IRHIPipeline.h"

#include <vulkan/vulkan.h>

class FVulkanRHIPipeline final : public IRHIPipeline {
public:
  FVulkanRHIPipeline(TRef<IRHIDevice> Device, TRef<IRHIRenderPass> RenderPass);
  ~FVulkanRHIPipeline();

private:
};
