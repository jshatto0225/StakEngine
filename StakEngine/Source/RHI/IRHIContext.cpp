#include "IRHIContext.h"

#ifdef SK_VULKAN
#include "FVulkanRHIContext.h"
using FPlatformRHIContext = FVulkanRHIGraphicsContext;
using FPlatformRHIDevce = FVulkanRHIDevice;
#endif

TRef<IRHIGraphicsContext> IRHIGraphicsContext::Create(TRef<IRHIDevice> Device) {
  return TCreateRef<FPlatformRHIContext>(std::static_pointer_cast<FPlatformRHIDevce>(Device));
}