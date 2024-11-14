#include "IRHIResource.h"

#ifdef SK_VULKAN
#include "FVulkanRHIResource.h"
#endif

TRef<IRHITexture> IRHITexture::Create(TRef<IRHIDevice> Device, FRHITextureDescription &Description) {
#ifdef SK_VULKAN
  return TCreateRef<FVulkanRHITexture>(Device, Description);
#endif
}