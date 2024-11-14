#include "FVulkanRHIRenderPass.h"

FVulkanRHIRenderPass::FVulkanRHIRenderPass(TRef<IRHIDevice> Device, const FRHIRenderPassDescription &Description) {
  mDescription = Description;


}