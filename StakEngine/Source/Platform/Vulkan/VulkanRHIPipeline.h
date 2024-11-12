#pragma once

#include "IRHIPipeline.h"

#include <vulkan/vulkan.h>

namespace Stak {

class VulkanRHIPipeline : public IRHIPipeline {
public:
  VulkanRHIPipeline(VkDevice device, PipelineDescription &desc);

  inline VkDescriptorPool getDescriptorPool() { return mDescriptorPool; }
  inline VkRenderPass getRenderPass() { return mRenderPass; }

private:
  VkDescriptorPool mDescriptorPool;
  VkRenderPass mRenderPass;
};

} // namespace Stak