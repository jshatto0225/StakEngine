#include "IRHIContext.h"

#ifdef SK_VULKAN
#include "FVulkanRHIContext.h"
#endif

TRef<IRHIGraphicsContext> IRHIGraphicsContext::Create(TRef<IRHIDevice> Device) {
#ifdef SK_VULKAN
  return TCreateRef<VulkanRHIGraphicsContext>(Device);
#endif
}