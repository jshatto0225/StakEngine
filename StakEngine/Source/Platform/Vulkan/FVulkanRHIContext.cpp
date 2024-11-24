#include "FVulkanRHIContext.h"

#include "FLog.h"
#include "IRHIResource.h"
#include "FVulkanRHIPipeline.h"
#include "FVulkanRHIResource.h"
#include "Asserts.h"

#include <backends/imgui_impl_vulkan.h>

FVulkanRHIGraphicsContext::FVulkanRHIGraphicsContext(TRef<FVulkanRHIDevice> Device) {
  mDevice = Device;

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
  AllocInfo.commandBufferCount = mDevice->GetMaxFramesInFlight();

  mCommandBuffers.resize(Device->GetMaxFramesInFlight());
  Err = vkAllocateCommandBuffers(mDevice->GetVkDevice(), &AllocInfo, mCommandBuffers.data());
  if (Err != VK_SUCCESS) {
    SK_LOG_ERROR("Failed to allocate command buffers");
  }
}

FVulkanRHIGraphicsContext::~FVulkanRHIGraphicsContext() {
  vkFreeCommandBuffers(mDevice->GetVkDevice(), mCommandPool, static_cast<FUInt32>(mCommandBuffers.size()), mCommandBuffers.data());
  vkDestroyCommandPool(mDevice->GetVkDevice(), mCommandPool, NULL);
}

void FVulkanRHIGraphicsContext::Begin() {
  VkResult Err = vkResetCommandBuffer(mCommandBuffers[mDevice->GetCurrentFrameIndex()], 0);

  VkCommandBufferBeginInfo BeginInfo = {};
  BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

  Err = vkBeginCommandBuffer(mCommandBuffers[mDevice->GetCurrentFrameIndex()], &BeginInfo);
}

void FVulkanRHIGraphicsContext::End() {
  vkEndCommandBuffer(mCommandBuffers[mDevice->GetCurrentFrameIndex()]);
}

void FVulkanRHIGraphicsContext::SetPipeline(TRef<IRHIPipeline> Pipeline) {
  vkCmdBindPipeline(mCommandBuffers[mDevice->GetCurrentFrameIndex()], VK_PIPELINE_BIND_POINT_GRAPHICS, std::static_pointer_cast<FVulkanRHIPipeline>(Pipeline)->GetVkPipeline());
}

void FVulkanRHIGraphicsContext::SetIndexBuffer(TRef<IRHIBuffer> Buffer) {
  vkCmdBindIndexBuffer(mCommandBuffers[mDevice->GetCurrentFrameIndex()], std::static_pointer_cast<FVulkanRHIBuffer>(Buffer)->GetVkBuffer(), Buffer->GetOffset(), VulkanRHIGetVkIndexType(Buffer->GetStride()));
}

void FVulkanRHIGraphicsContext::SetVertexBuffers(std::vector<TRef<IRHIBuffer>> Buffers) {
  std::vector<VkBuffer> VulkanBuffers;
  VulkanBuffers.reserve(Buffers.size());

  std::vector<FUInt64> Offsets;
  Offsets.reserve(Buffers.size());

  for (const TRef<IRHIBuffer> &Buffer : Buffers) {
    VulkanBuffers.push_back(std::static_pointer_cast<FVulkanRHIBuffer>(Buffer)->GetVkBuffer());
    Offsets.push_back(Buffer->GetOffset());
  }

  vkCmdBindVertexBuffers(mCommandBuffers[mDevice->GetCurrentFrameIndex()], 0, static_cast<FUInt32>(VulkanBuffers.size()), VulkanBuffers.data(), Offsets.data());
}

void FVulkanRHIGraphicsContext::SetStreamOutputTargets(std::vector<TRef<IRHITexture>> Textures) {
  // TODO:
  SK_LOG_ERROR("SetStreamOutputTargets not implemented for vulkan");
}

void FVulkanRHIGraphicsContext::SetRenderTargets(std::vector<TRef<IRHITexture>> Textures, const FRHIRect &RenderArea) {
  std::vector<VkRenderingAttachmentInfo> ColorAttachments = {};
  ColorAttachments.reserve(Textures.size());

  std::optional<VkRenderingAttachmentInfo> DepthAttachment = {};
  std::optional<VkRenderingAttachmentInfo> StencilAttachment = {};

  FUInt32 MinLayers = UINT32_MAX;
  for (const TRef<IRHITexture> &Texture : Textures) {
    TRef<FVulkanRHITexture> vulkanTexture = std::static_pointer_cast<FVulkanRHITexture>(Texture);

    FUInt32 Layers = Texture->GetLayers();
    if (Layers < MinLayers) {
      MinLayers = Layers;
    }

    VkRenderingAttachmentInfo Attachment = {};
    Attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    Attachment.clearValue = VulkanRHIGetVkClearValue(vulkanTexture->GetDescription().ClearValue);
    Attachment.imageLayout = VulkanRHIGetVkImageLayout(vulkanTexture->GetDescription().Usage);
    Attachment.imageView = vulkanTexture->GetVkImageView();
    Attachment.storeOp = VulkanRHIGetVkStoreOp(vulkanTexture->GetDescription().StoreOp);
    Attachment.loadOp = VulkanRHIGetVkLoadOp(vulkanTexture->GetDescription().LoadOp);
    // TODO: What is a resolve
    Attachment.resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    Attachment.resolveImageView = VK_NULL_HANDLE;
    Attachment.resolveMode = VK_RESOLVE_MODE_NONE;

    if (Attachment.imageLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
      ColorAttachments.push_back(Attachment);
    }
    else if (Attachment.imageLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
      DepthAttachment = Attachment;
      StencilAttachment = Attachment;
    }
    else {
      SK_LOG_ERROR("Invalid Texture usage for render target");
    }
  }

  VkRenderingInfo RenderInfo = {};
  RenderInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
  RenderInfo.renderArea.offset = { static_cast<FSInt32>(RenderArea.X), static_cast<FSInt32>(RenderArea.Y) };
  RenderInfo.renderArea.extent = { RenderArea.Width, RenderArea.Height };
  RenderInfo.layerCount = MinLayers;
  RenderInfo.viewMask = 0x01; // TODO: Make this dynamic?
  RenderInfo.colorAttachmentCount = static_cast<FUInt32>(ColorAttachments.size());
  RenderInfo.pColorAttachments = ColorAttachments.data();
  if (DepthAttachment.has_value()) {
    RenderInfo.pDepthAttachment = &DepthAttachment.value();
  }
  if (StencilAttachment.has_value()) {
    RenderInfo.pDepthAttachment = &StencilAttachment.value();
  }

  vkCmdBeginRendering(mCommandBuffers[mDevice->GetCurrentFrameIndex()], &RenderInfo);
}

void FVulkanRHIGraphicsContext::UnsetRenderTargets() {
  vkCmdEndRendering(mCommandBuffers[mDevice->GetCurrentFrameIndex()]);
}

void FVulkanRHIGraphicsContext::SetDescriptorSet(TRef<IRHIDescritporSet> Set) {
  // TODO:
  SK_LOG_ERROR("SetDescriptorSet not implemented for vulkan");
}

void FVulkanRHIGraphicsContext::SetViewports(const std::vector<FRHIViewport> &Viewports) {
  std::vector<VkViewport> VulkanViewports;
  VulkanViewports.reserve(Viewports.size());

  for (const FRHIViewport &Viewport : Viewports) {
    VkViewport VulkanViewport = {};
    VulkanViewport.x = Viewport.X;
    VulkanViewport.y = Viewport.Y;
    VulkanViewport.width = Viewport.Width;
    VulkanViewport.height = Viewport.Height;
    VulkanViewport.minDepth = Viewport.MinDepth;
    VulkanViewport.maxDepth = Viewport.MaxDepth;

    VulkanViewports.push_back(VulkanViewport);
  }

  vkCmdSetViewport(mCommandBuffers[mDevice->GetCurrentFrameIndex()], 0, static_cast<FUInt32>(VulkanViewports.size()), VulkanViewports.data());
}

void FVulkanRHIGraphicsContext::SetScissors(const std::vector<FRHIRect> &Scissors) {
  std::vector<VkRect2D> VulkanScissors;
  VulkanScissors.reserve(Scissors.size());

  for (const FRHIRect &Scissor : Scissors) {
    VkRect2D VulkanScissor = {};
    VulkanScissor.offset.x = Scissor.X;
    VulkanScissor.offset.y = Scissor.Y;
    VulkanScissor.extent.width = Scissor.Width;
    VulkanScissor.extent.height = Scissor.Height;

    VulkanScissors.push_back(VulkanScissor);
  }

  vkCmdSetScissor(mCommandBuffers[mDevice->GetCurrentFrameIndex()], 0, static_cast<FUInt32>(VulkanScissors.size()), VulkanScissors.data());
}

void FVulkanRHIGraphicsContext::SetBlendConstants(std::array<FFloat, 4> Constants) {
  const FFloat BlendConstants[] = {
    Constants[0],
    Constants[0],
    Constants[0],
    Constants[0]
  };
  vkCmdSetBlendConstants(mCommandBuffers[mDevice->GetCurrentFrameIndex()], BlendConstants);
}

void FVulkanRHIGraphicsContext::SetDepthStencilReferenceValue(FUInt32 Val) {
  vkCmdSetStencilReference(mCommandBuffers[mDevice->GetCurrentFrameIndex()], VK_STENCIL_FACE_FRONT_AND_BACK, Val);
}

void FVulkanRHIGraphicsContext::SetTopology(ERHITopology Topology) {
  vkCmdSetPrimitiveTopology(mCommandBuffers[mDevice->GetCurrentFrameIndex()], VulkanRHIGetVkTopology(Topology));
}

void FVulkanRHIGraphicsContext::ResourceBarrier(const FRHIResourceBarrierDescription &Description) {
  std::vector<VkImageMemoryBarrier2> ImageBarriers = {};
  ImageBarriers.reserve(Description.Transitions.size());

  for (const FRHITransitionBarrierDescription &Transition : Description.Transitions) {
    ASSERT(Transition.Resource->GetType() == ERHIResourceType::TEXTURE);

    TRef<FVulkanRHITexture> Texture = std::static_pointer_cast<FVulkanRHITexture>(Transition.Resource);

    ASSERT(Texture->IsSwapchainImage());

    VkImageMemoryBarrier2 ImageBarrier = {};
    ImageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
    ImageBarrier.image = Texture->GetVkImage();
    ImageBarrier.srcQueueFamilyIndex = mDevice->GetVkGraphicsQueueFamilyIndex();
    ImageBarrier.dstQueueFamilyIndex = mDevice->GetVkGraphicsQueueFamilyIndex();
    ImageBarrier.oldLayout = VulkanRHIGetVkImageLayout(Transition.Resource->GetUsage());
    ImageBarrier.newLayout = VulkanRHIGetVkImageLayout(Transition.NewUsage);
    ImageBarrier.srcStageMask = VulkanRHIGetVkPipelineStageFlags2(Transition.Resource->GetUsage());
    ImageBarrier.srcAccessMask = VulkanRHIGetVkAccessFlagBits2(Transition.Resource->GetUsage());
    ImageBarrier.dstStageMask = VulkanRHIGetVkPipelineStageFlags2(Transition.NewUsage);
    ImageBarrier.dstAccessMask = VulkanRHIGetVkAccessFlagBits2(Transition.NewUsage);

    if (Texture->IsSwapchainImage()) {
      ImageBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      ImageBarrier.subresourceRange.baseMipLevel = 0;
      ImageBarrier.subresourceRange.levelCount = 1;
      ImageBarrier.subresourceRange.baseArrayLayer = 0;
      ImageBarrier.subresourceRange.layerCount = 1;
    }
    else {
      SK_LOG_ERROR("Non swapchain image transtions not implemented");
    }

    ImageBarriers.push_back(ImageBarrier);

    Transition.Resource->SetUsage(Transition.NewUsage);
  }

  VkDependencyInfo DependencyInfo = {};
  DependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;

  // TODO:
  DependencyInfo.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

  DependencyInfo.imageMemoryBarrierCount = ImageBarriers.size();
  DependencyInfo.pImageMemoryBarriers = ImageBarriers.data();

  vkCmdPipelineBarrier2(mCommandBuffers[mDevice->GetCurrentFrameIndex()], &DependencyInfo);
}

void FVulkanRHIGraphicsContext::DrawIndexed(FUInt32 FirstIndex, FUInt32 IndexCount, FUInt32 FirstInstance, FUInt32 InstanceCount, FSInt32 VertexOffset) {
  vkCmdDrawIndexed(mCommandBuffers[mDevice->GetCurrentFrameIndex()], IndexCount, InstanceCount, FirstIndex, VertexOffset, FirstInstance);
}

void FVulkanRHIGraphicsContext::DrawInstanced(FUInt32 FirstVertex, FUInt32 VertexCount, FUInt32 FirstInstance, FUInt32 InstanceCount) {
  vkCmdDraw(mCommandBuffers[mDevice->GetCurrentFrameIndex()], VertexCount, InstanceCount, FirstVertex, FirstInstance);
}