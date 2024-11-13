#include "VulkanRHIContext.h"

#include "Log.h"

namespace Stak {

VulkanRHIGraphicsContext::VulkanRHIGraphicsContext(VkDevice device, u32 graphicsQueueFamily) : mCommandBuffer(VK_NULL_HANDLE) {
  mDevice = device;

  VkCommandPoolCreateInfo poolInfo = {};
  poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  poolInfo.queueFamilyIndex = graphicsQueueFamily;

  VkResult r = vkCreateCommandPool(device, &poolInfo, nullptr, &mCommandPool);
  if (r != VK_SUCCESS) {
    SK_LOG_ERROR("Failed to create command pool");
  }

  VkCommandBufferAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = mCommandPool;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = 1;

  r = vkAllocateCommandBuffers(device, &allocInfo, &mCommandBuffer);
  if (r != VK_SUCCESS) {
    SK_LOG_ERROR("Failed to allocate command buffers");
  }
}

VulkanRHIGraphicsContext::~VulkanRHIGraphicsContext() {
  vkFreeCommandBuffers(mDevice, mCommandPool, 1, &mCommandBuffer);
  vkDestroyCommandPool(mDevice, mCommandPool, NULL);
}

void VulkanRHIGraphicsContext::begin() {
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = 0;
  beginInfo.pInheritanceInfo = NULL;

  VkResult r = vkBeginCommandBuffer(mCommandBuffer, &beginInfo);
  if (r != VK_SUCCESS) {
    SK_LOG_ERROR("Failed to begin command buffer");
  }
}

void VulkanRHIGraphicsContext::end() {
  vkEndCommandBuffer(mCommandBuffer);
}

void VulkanRHIGraphicsContext::resourceBarrier(ResourceBarrierDescription &barrierDesc) {

}

void VulkanRHIGraphicsContext::setPipeline(Ref<IRHIPipeline> pipeline) {
  pipeline->bind();
}

void VulkanRHIGraphicsContext::setVertexBuffer(Ref<IRHIBuffer> buffer) {

}

void VulkanRHIGraphicsContext::setIndexBuffer(Ref<IRHIBuffer> buffer) {

}

void VulkanRHIGraphicsContext::setViewport(i32 x, i32 y, i32 width, i32 height) {

}

void VulkanRHIGraphicsContext::draw() {

}

VulkanRHIComputeContext::VulkanRHIComputeContext() {

}

VulkanRHIComputeContext::~VulkanRHIComputeContext() {

}

void VulkanRHIComputeContext::begin() {

}

void VulkanRHIComputeContext::end() {

}

void VulkanRHIComputeContext::resourceBarrier(ResourceBarrierDescription &barrierDesc) {

}

void VulkanRHIComputeContext::setPipeline(Ref<IRHIPipeline> pipeline) {

}

void VulkanRHIComputeContext::dispatch() {

}

VulkanRHIUploadContext::VulkanRHIUploadContext() {

}

VulkanRHIUploadContext::~VulkanRHIUploadContext() {

}

void VulkanRHIUploadContext::begin() {

}

void VulkanRHIUploadContext::end() {

}

void VulkanRHIUploadContext::resourceBarrier(ResourceBarrierDescription &barrierDesc) {

}

void VulkanRHIUploadContext::uploadBuffer(Ref<IRHIBuffer> buffer, void *data) {

}

void VulkanRHIUploadContext::uploadTexture(Ref<IRHITexture> texture, void *data) {

}

} // namespace Stak