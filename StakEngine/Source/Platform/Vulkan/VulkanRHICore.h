#pragma once

#include "RHICore.h"

#include <vulkan/vulkan.h>

VkFormat VulkanRHIGetVkFormat(ERHIFormat Format);
VkSampleCountFlagBits VulkanRHIGetVkSampleCount(ERHISampleCount SampleCount);
VkAttachmentLoadOp VulkanRHIGetVkLoadOp(ERHILoadOp LoadOp);
VkAttachmentStoreOp VulkanRHIGetVkStoreOp(ERHIStoreOp StoreOp);
VkImageLayout VulkanRHIGetVkImageLayout(ERHIImageState State);
VkPipelineBindPoint VulkanRHIGetVkPipelineBindPoint(ERHIPipelineBindPoint BindPoint);
VkPipelineStageFlags VulkanRHIGetVkPipelineStage(ERHIPipelineStage Stage);
VkAccessFlags VulkanRHIGetVkAccessMask(ERHIAccess Access);