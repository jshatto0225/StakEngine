#pragma once

#include "RHICore.h"

#include <vulkan/vulkan.h>

VkFormat VulkanRHIGetVkFormat(ERHIFormat Format);
VkSampleCountFlagBits VulkanRHIGetVkSampleCount(ERHISampleCount SampleCount);
VkAttachmentLoadOp VulkanRHIGetVkLoadOp(ERHILoadOp LoadOp);
VkAttachmentStoreOp VulkanRHIGetVkStoreOp(ERHIStoreOp StoreOp);
VkImageLayout VulkanRHIGetVkImageLayout(ERHIResourceUsage Usage);
VkAccessFlagBits2 VulkanRHIGetVkAccessFlagBits2(ERHIResourceUsage Usage);
VkPipelineStageFlags2 VulkanRHIGetVkPipelineStageFlags2(ERHIResourceUsage Usage);
VkPipelineBindPoint VulkanRHIGetVkPipelineBindPoint(ERHIPipelineBindPoint BindPoint);
VkClearValue VulkanRHIGetVkClearValue(FRHIClearValue ClearValue);
ERHIFormat VulkanRHIGetERHIFormat(VkFormat Format);
VkPrimitiveTopology VulkanRHIGetVkTopology(ERHITopology Topology);
VkIndexType VulkanRHIGetVkIndexType(FUInt32 Stride);

class FVulkanRHIWorkRecipt : public IRHIWorkRecipt {
public:
  FVulkanRHIWorkRecipt(VkFence Fence) { mFence = Fence; }

  inline VkFence GetFence() const { return mFence; }

private:
  VkFence mFence;
};