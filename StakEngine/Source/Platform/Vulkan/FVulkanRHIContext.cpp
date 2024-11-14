#include "FVulkanRHIContext.h"

#include "FLog.h"

VulkanRHIGraphicsContext::VulkanRHIGraphicsContext(TRef<IRHIDevice> Device) {
  mDevice = std::static_pointer_cast<FVulkanRHIDevice>(Device);

  VkCommandPoolCreateInfo PoolInfo = {};

  PoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  PoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  PoolInfo.queueFamilyIndex = mDevice->GetGraphicsQueueFamilyIndex();

  VkResult r = vkCreateCommandPool(mDevice->GetVkDevice(), &PoolInfo, NULL, &mCommandPool);
  if (r != VK_SUCCESS) {
    SK_LOG_ERROR("Failed to create command pool");
  }

  VkCommandBufferAllocateInfo AllocInfo = {};
  AllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  AllocInfo.commandPool = mCommandPool;
  AllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  AllocInfo.commandBufferCount = MAX_FRAMES_IN_FLIGHT;

  VkResult r = vkAllocateCommandBuffers(mDevice->GetVkDevice(), &AllocInfo, &mCommandBuffer);
  if (r != VK_SUCCESS) {
    SK_LOG_ERROR("Failed to allocate command buffers");
  }
}

VulkanRHIGraphicsContext::~VulkanRHIGraphicsContext() {
  vkFreeCommandBuffers(mDevice->GetVkDevice(), mCommandPool, 1, &mCommandBuffer);
  vkDestroyCommandPool(mDevice->GetVkDevice(), mCommandPool, NULL);
}

void VulkanRHIGraphicsContext::Begin() {
  vkResetCommandBuffer(mCommandBuffer, 0);
}

void VulkanRHIGraphicsContext::End() {
  vkEndCommandBuffer(mCommandBuffer);
}

void VulkanRHIGraphicsContext::SetRenderPass(TRef<IRHIRenderPass> RenderPass) {

}

void VulkanRHIGraphicsContext::SetPipeline(TRef<IRHIPipeline> Pipeline) {

}

void VulkanRHIGraphicsContext::SetVertexBuffer(TRef<IRHIBuffer> Buffer) {

}

void VulkanRHIGraphicsContext::SetIndexBuffer(TRef<IRHIBuffer> Buffer) {

}

void VulkanRHIGraphicsContext::SetViewport(FSInt32 X, FSInt32 Y, FSInt32 Width, FSInt32 Height) {

}

void VulkanRHIGraphicsContext::Draw() {
}