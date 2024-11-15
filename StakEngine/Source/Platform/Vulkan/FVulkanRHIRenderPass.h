#pragma once

#include "IRHIRenderPass.h"

#include "FVulkanRHIDevice.h"
#include "FVulkanRHIFramebuffer.h"

class FVulkanRHIRenderPass final : public IRHIRenderPass {
public:
  FVulkanRHIRenderPass(TRef<FVulkanRHIDevice> Device, TRef<FVulkanRHIFramebuffer> InitialFramebuffer, const FRHIRenderPassDescription &Desctiption);
  ~FVulkanRHIRenderPass();

  void SetFramebuffer(TRef<IRHIFramebuffer> Framebuffer);

public:
  VkFramebuffer GetVkFramebuffer() { return mFramebuffer->GetVkFramebuffer(); }
  VkRenderPass GetVkRenderPass() { return mRenderPass; }
  VkRect2D GetVkRenderArea(FUInt32 SubpassIndex);

private:
  FRHIRenderPassDescription mDescription;

private:
  TRef<FVulkanRHIDevice> mDevice;
  TRef<FVulkanRHIFramebuffer> mFramebuffer;

private:
  VkRenderPass mRenderPass;
};