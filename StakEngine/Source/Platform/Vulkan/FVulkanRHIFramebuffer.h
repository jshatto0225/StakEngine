#pragma once

#include "IRHIFramebuffer.h"
#include "FVulkanRHIDevice.h"
#include "FVulkanRHIResource.h"

class FVulkanRHIFramebuffer final : public IRHIFramebuffer {
public:
  FVulkanRHIFramebuffer(TRef<FVulkanRHIDevice> Device, std::vector<TRef<FVulkanRHITexture>> Textures);
  ~FVulkanRHIFramebuffer();

public:
  VkFramebuffer GetVkFramebuffer() { return mFramebuffer; }

private:
  TRef<FVulkanRHIDevice> mDevice;
  std::vector<TRef<FVulkanRHITexture>> mTextures;

private:
  VkFramebuffer mFramebuffer;
};