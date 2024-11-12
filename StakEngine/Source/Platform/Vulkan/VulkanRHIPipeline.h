#pragma once

#include "IRHIPipeline.h"

#include <vulkan/vulkan.h>

namespace Stak {

class VulkanRHIPipeline : public IRHIPipeline {
public:
  VulkanRHIPipeline(VkDevice device, RHIPipelineDescription &desc);

  inline VkDescriptorPool getDescriptorPool() { return mDescriptorPool; }
  inline VkRenderPass getRenderPass() { return mRenderPass; }

  VkFramebuffer getRenderTarget() { return mFramebuffer; }
  VkExtent2D getRenderAreaExtent() { return mRenderAreaExtent; }

private:
  VkDescriptorPool mDescriptorPool;
  VkRenderPass mRenderPass;
  VkFramebuffer mFramebuffer;
  VkExtent2D mRenderAreaExtent;
};

} // namespace Stak