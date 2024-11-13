#include "VulkanRHIPipeline.h"

namespace Stak {

VulkanRHIPipeline::VulkanRHIPipeline(VkDevice device, const RHIPipelineDescription &desc) {

}

VulkanRHIPipeline::~VulkanRHIPipeline() {

}

void VulkanRHIPipeline::bind() {
  VkRenderPassBeginInfo renderPassInfo = {};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = mRenderPass;
  renderPassInfo.renderArea.offset = { 0, 0 };
  renderPassInfo.renderArea.extent = mRenderAreaExtent;
  renderPassInfo.framebuffer = mFramebuffer;
  renderPassInfo.pClearValues = mClearValues.data();
  renderPassInfo.clearValueCount = mClearValues.size();
}

} // namespace Stak