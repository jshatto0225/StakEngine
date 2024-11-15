#include "IRHIContext.h"

#ifdef SK_VULKAN
#include "FVulkanRHIContext.h"
typedef FVulkanRHIGraphicsContext FPlatformRHIContext;
typedef FVulkanRHIDevice FPlatformRHIDevce;
#endif

TRef<IRHIGraphicsContext> IRHIGraphicsContext::Create(TRef<IRHIDevice> Device) {
  return TCreateRef<FPlatformRHIContext>(std::static_pointer_cast<FPlatformRHIDevce>(Device));
}