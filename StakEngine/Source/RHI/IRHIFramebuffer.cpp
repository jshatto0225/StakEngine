#include "IRHIFramebuffer.h"

#ifdef SK_VULKAN
#include "FVulkanRHIFramebuffer.h"
typedef FVulkanRHIFramebuffer FPlatformRHIFramebuffer;
typedef FVulkanRHITexture FPlatformRHITexture;
typedef FVulkanRHIDevice FPlatformRHIDevice;
typedef FVulkanRHIRenderPass FPlatformRHIRenderPass;
#endif

TRef<IRHIFramebuffer> IRHIFramebuffer::Create(TRef<IRHIDevice> Device, std::vector<TRef<IRHITexture>> Textures, TRef<IRHIRenderPass> RenderPass) {
  std::vector<TRef<FPlatformRHITexture>> PlatformTextures;

  for (TRef<IRHITexture> Texture : Textures) {
    PlatformTextures.push_back(std::static_pointer_cast<FPlatformRHITexture>(Texture));
  }

  return TCreateRef<FPlatformRHIFramebuffer>(std::static_pointer_cast<FPlatformRHIDevice>(Device), PlatformTextures, std::static_pointer_cast<FPlatformRHIRenderPass>(RenderPass));
}