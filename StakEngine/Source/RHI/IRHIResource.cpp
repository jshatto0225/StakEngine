#include "IRHIResource.h"

#ifdef SK_VULKAN
#include "FVulkanRHIResource.h"
typedef FVulkanRHITexture FPlatformRHITexture;
typedef FVulkanRHIDevice FPlatformRHIDevice;
#endif

TRef<IRHITexture> IRHITexture::Create(TRef<IRHIDevice> Device, FRHITextureDescription &Description) {
  return TCreateRef<FPlatformRHITexture>(std::static_pointer_cast<FPlatformRHIDevice>(Device), Description);
}