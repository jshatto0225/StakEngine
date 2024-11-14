#pragma once

#include "IRHIRenderPass.h"

#include "FVulkanRHIDevice.h"

class FVulkanRHIRenderPass final : public IRHIRenderPass {
public:
  FVulkanRHIRenderPass(TRef<IRHIDevice> Device, const FRHIRenderPassDescription &Desctiption);

private:
  FRHIRenderPassDescription mDescription;

private:
  TRef<FVulkanRHIDevice> mDevice;

private:
  std::vector<VkFramebuffer> mFramebuffers;
  VkRenderPass mRenderPass;
};