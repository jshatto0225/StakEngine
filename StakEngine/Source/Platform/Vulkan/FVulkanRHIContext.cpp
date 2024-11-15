#include "FVulkanRHIContext.h"

#include "FLog.h"
#include "FVulkanRHIRenderPass.h"

FVulkanRHIGraphicsContext::FVulkanRHIGraphicsContext(TRef<FVulkanRHIDevice> Device) {
  mDevice = Device;

  VkCommandPoolCreateInfo PoolInfo = {};

  PoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  PoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  PoolInfo.queueFamilyIndex = mDevice->GetGraphicsQueueFamilyIndex();

  VkResult Err = vkCreateCommandPool(mDevice->GetVkDevice(), &PoolInfo, NULL, &mCommandPool);
  if (Err != VK_SUCCESS) {
    SK_LOG_ERROR("Failed to create command pool");
  }

  VkCommandBufferAllocateInfo AllocInfo = {};
  AllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  AllocInfo.commandPool = mCommandPool;
  AllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  AllocInfo.commandBufferCount = MAX_FRAMES_IN_FLIGHT;

  Err = vkAllocateCommandBuffers(mDevice->GetVkDevice(), &AllocInfo, &mCommandBuffer);
  if (Err != VK_SUCCESS) {
    SK_LOG_ERROR("Failed to allocate command buffers");
  }
}

FVulkanRHIGraphicsContext::~FVulkanRHIGraphicsContext() {
  vkFreeCommandBuffers(mDevice->GetVkDevice(), mCommandPool, 1, &mCommandBuffer);
  vkDestroyCommandPool(mDevice->GetVkDevice(), mCommandPool, NULL);
}

void FVulkanRHIGraphicsContext::Begin() {
  vkResetCommandBuffer(mCommandBuffer, 0);
}

void FVulkanRHIGraphicsContext::End() {
  vkEndCommandBuffer(mCommandBuffer);
}

void FVulkanRHIGraphicsContext::SetRenderPass(TRef<IRHIRenderPass> RenderPass, FRHIRenderArea RenderArea) {
  TRef<FVulkanRHIRenderPass> VulkanRenderPass = std::static_pointer_cast<FVulkanRHIRenderPass>(RenderPass);

  VkRenderPassBeginInfo BeginInfo = {};
  BeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  BeginInfo.renderPass = VulkanRenderPass->GetVkRenderPass();
  BeginInfo.clearValueCount = 0; // TODO
  BeginInfo.pClearValues = 0; // TODO
  BeginInfo.framebuffer = VulkanRenderPass->GetVkFramebuffer();
  BeginInfo.renderArea.offset = { static_cast<FSInt32>(RenderArea.X), static_cast<FSInt32>(RenderArea.Y) };
  BeginInfo.renderArea.extent = { RenderArea.Width, RenderArea.Height };

  vkCmdBeginRenderPass(mCommandBuffer, &BeginInfo, VK_SUBPASS_CONTENTS_INLINE /* TODO */);
}

void FVulkanRHIGraphicsContext::SetPipeline(TRef<IRHIPipeline> Pipeline) {

}

void FVulkanRHIGraphicsContext::SetVertexBuffer(TRef<IRHIBuffer> Buffer) {

}

void FVulkanRHIGraphicsContext::SetIndexBuffer(TRef<IRHIBuffer> Buffer) {

}

void FVulkanRHIGraphicsContext::SetViewport(FSInt32 X, FSInt32 Y, FSInt32 Width, FSInt32 Height) {

}

void FVulkanRHIGraphicsContext::Draw() {
}