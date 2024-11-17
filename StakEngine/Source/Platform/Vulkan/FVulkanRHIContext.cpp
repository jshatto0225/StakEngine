#include "FVulkanRHIContext.h"

#include "FLog.h"
#include "IRHIResource.h"
#include "FVulkanRHIResource.h"
#include "FVulkanRHIPipeline.h"

#include <backends/imgui_impl_vulkan.h>

FVulkanRHIGraphicsContext::FVulkanRHIGraphicsContext(TRef<FVulkanRHIDevice> Device, FUInt32 MaxFramesInFlight) {
  mDevice = Device;
  mIsRendering = false;

  VkCommandPoolCreateInfo PoolInfo = {};

  PoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  PoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  PoolInfo.queueFamilyIndex = mDevice->GetVkGraphicsQueueFamilyIndex();

  VkResult Err = vkCreateCommandPool(mDevice->GetVkDevice(), &PoolInfo, NULL, &mCommandPool);
  if (Err != VK_SUCCESS) {
    SK_LOG_ERROR("Failed to create command pool");
  }

  VkCommandBufferAllocateInfo AllocInfo = {};
  AllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  AllocInfo.commandPool = mCommandPool;
  AllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  AllocInfo.commandBufferCount = MaxFramesInFlight;

  Err = vkAllocateCommandBuffers(mDevice->GetVkDevice(), &AllocInfo, mCommandBuffers.data());
  if (Err != VK_SUCCESS) {
    SK_LOG_ERROR("Failed to allocate command buffers");
  }
}

FVulkanRHIGraphicsContext::~FVulkanRHIGraphicsContext() {
  vkFreeCommandBuffers(mDevice->GetVkDevice(), mCommandPool, mCommandBuffers.size(), mCommandBuffers.data());
  vkDestroyCommandPool(mDevice->GetVkDevice(), mCommandPool, NULL);
}

void FVulkanRHIGraphicsContext::Begin() {
  vkResetCommandBuffer(mCommandBuffers[mDevice->GetCurrentFrameIndex()], 0);
}

void FVulkanRHIGraphicsContext::End() {
  if (mIsRendering) {
    vkCmdEndRendering(mCommandBuffers[mDevice->GetCurrentFrameIndex()]);
    mIsRendering = false;
  }
  vkEndCommandBuffer(mCommandBuffers[mDevice->GetCurrentFrameIndex()]);
}

void FVulkanRHIGraphicsContext::SetPipeline(TRef<IRHIPipeline> Pipeline) {
  vkCmdBindPipeline(mCommandBuffers[mDevice->GetCurrentFrameIndex()], VK_PIPELINE_BIND_POINT_GRAPHICS, std::static_pointer_cast<FVulkanRHIPipeline>(Pipeline)->GetVkPipeline());
}

void FVulkanRHIGraphicsContext::SetIndexBuffer(TRef<IRHIBuffer> Buffer) {
  vkCmdBindIndexBuffer(mCommandBuffers[mDevice->GetCurrentFrameIndex()], std::static_pointer_cast<FVulkanRHIBuffer>(Buffer)->GetVkBuffer());
}

void FVulkanRHIGraphicsContext::SetVertexBuffers(std::vector<TRef<IRHIBuffer>> Buffers) {
  vkCmdBindVertexBuffers(mCommandBuffers[mDevice->GetCurrentFrameIndex()]);
}

void FVulkanRHIGraphicsContext::SetStreamOutputTargets(std::vector<TRef<IRHITexture>> Textures) {

}

void FVulkanRHIGraphicsContext::SetRenderTargets(std::vector<TRef<IRHITexture>> Textures, const FRHIRect &RenderArea) {
  if (mIsRendering) {
    vkCmdEndRendering(mCommandBuffers[mDevice->GetCurrentFrameIndex()]);
    mIsRendering = false;
  }

  std::vector<VkRenderingAttachmentInfo> ColorAttachments = {};
  VkRenderingAttachmentInfo DepthAttachment = {};
  VkRenderingAttachmentInfo StencilAttachment = {};

  FUInt32 MinLayers = UINT32_MAX;
  for (TRef<IRHITexture> Texture : Textures) {
    TRef<FVulkanRHITexture> vulkanTexture = std::static_pointer_cast<FVulkanRHITexture>(Texture);

    FUInt32 Layers = Texture->GetLayers();
    if (Layers < MinLayers) {
      MinLayers = Layers;
    }

    VkRenderingAttachmentInfo Attachment = {};
    Attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    Attachment.clearValue = VulkanRHIGetVkClearValue(vulkanTexture->GetDescription().ClearValue);
    Attachment.imageLayout = VulkanRHIGetVkImageLayout(vulkanTexture->GetDescription().Usage);
    Attachment.imageView = vulkanTexture->GetVkImageViews()[mDevice->GetCurrentFrameIndex()];
    Attachment.storeOp = VulkanRHIGetVkStoreOp(vulkanTexture->GetDescription().StoreOp);
    Attachment.loadOp = VulkanRHIGetVkLoadOp(vulkanTexture->GetDescription().LoadOp);
    Attachment.resolveImageLayout = ;
    Attachment.resolveImageView = ;
    Attachment.resolveMode = ;

    if (Attachment.imageLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
      ColorAttachments.push_back(Attachment);
    }
    else if (Attachment.imageLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
      DepthAttachment = Attachment;
      StencilAttachment = Attachment;
    }
    else {
      // Error
    }
  }

  VkRenderingInfo RenderInfo = {};
  RenderInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
  RenderInfo.renderArea.offset = { static_cast<FSInt32>(RenderArea.X), static_cast<FSInt32>(RenderArea.Y) };
  RenderInfo.renderArea.extent = { RenderArea.Width, RenderArea.Height };
  RenderInfo.layerCount = MinLayers;
  RenderInfo.viewMask = 0; // TODO
  RenderInfo.colorAttachmentCount = ColorAttachments.size();
  RenderInfo.pColorAttachments = ColorAttachments.data();
  RenderInfo.pDepthAttachment = &DepthAttachment;
  RenderInfo.pStencilAttachment = &StencilAttachment;

  vkCmdBeginRenderingKHR(mCommandBuffers[mDevice->GetCurrentFrameIndex()], &RenderInfo);
}

void FVulkanRHIGraphicsContext::SetDescriptorSet(TRef<IRHIDescritporSet> Set) {
  vkCmdBindDescriptorSets(mCommandBuffers[mDevice->GetCurrentFrameIndex()], VK_PIPELINE_BIND_POINT_GRAPHICS);
}

void FVulkanRHIGraphicsContext::SetViewports(const std::vector<FRHIRect> &Viewports) {
  vkCmdSetViewport(mCommandBuffers[mDevice->GetCurrentFrameIndex()]);
}

void FVulkanRHIGraphicsContext::SetScissors(const std::vector<FRHIRect> &Scissors) {
  vkCmdSetScissor(mCommandBuffers[mDevice->GetCurrentFrameIndex()]);
}

void FVulkanRHIGraphicsContext::SetBlendConstants(std::array<FFloat, 4> Constants) {
  vkCmdSetBlendConstants(mCommandBuffers[mDevice->GetCurrentFrameIndex()]);
}

void FVulkanRHIGraphicsContext::SetDepthStencilReferenceValue(FUInt32 Val) {
  vkCmdSetStencilReference(mCommandBuffers[mDevice->GetCurrentFrameIndex()], , Val);
}

void FVulkanRHIGraphicsContext::SetTopology(ERHITopology Topology) {
  vkCmdSetPrimitiveTopology(mCommandBuffers[mDevice->GetCurrentFrameIndex()], VulkanRHIGetVkTopology(Topology));
}

void FVulkanRHIGraphicsContext::ResourceBarrier(const FRHIResourceBarrierDescription &Description) {
  vkCmdPipelineBarrier(mCommandBuffers[mDevice->GetCurrentFrameIndex()]);
}

void FVulkanRHIGraphicsContext::Draw() {
  vkCmdDrawIndexed(mCommandBuffers[mDevice->GetCurrentFrameIndex()]);
}