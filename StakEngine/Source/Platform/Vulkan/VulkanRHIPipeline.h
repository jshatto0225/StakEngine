#pragma once

#include "IRHIPipeline.h"

#include <vulkan/vulkan.h>

namespace Stak {

class VulkanRHIPipeline : public IRHIPipeline {
public:
  VulkanRHIPipeline(VkDevice device, const RHIPipelineDescription &desc);
  ~VulkanRHIPipeline();

  void bind();

public:
  inline VkDescriptorPool getDescriptorPool() { return mDescriptorPool; }
  inline VkRenderPass getRenderPass() { return mRenderPass; }
  inline VkFramebuffer getRenderTarget() { return mFramebuffer; }
  inline VkExtent2D getRenderAreaExtent() { return mRenderAreaExtent; }
  inline const std::vector<VkClearValue> &getClearValues() { return mClearValues; }

private:
  VkDescriptorPool mDescriptorPool;
  VkRenderPass mRenderPass;
  VkFramebuffer mFramebuffer;
  VkExtent2D mRenderAreaExtent;
  std::vector<VkClearValue> mClearValues;
};

} // namespace Stak