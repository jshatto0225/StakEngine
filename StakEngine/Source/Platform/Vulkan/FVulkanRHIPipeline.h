#pragma once

#include "IRHIPipeline.h"

#include <vulkan/vulkan.h>

class VulkanRHIPipeline final : public IRHIPipeline {
public:
  VulkanRHIPipeline(TRef<IRHIDevice> Device, TRef<IRHIRenderPass> RenderPass);
  ~VulkanRHIPipeline();

private:
};
