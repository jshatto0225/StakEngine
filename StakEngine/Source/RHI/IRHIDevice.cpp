#include "IRHIDevice.h"

#ifdef SK_VULKAN
#include "FVulkanRHIDevice.h"
#endif

TRef<IRHIDevice> IRHIDevice::Create(TRef<IRHIInstance> Instance, TRef<IWindow> Window) {
#ifdef SK_VULKAN
  return TCreateRef<FVulkanRHIDevice>(Instance, Window);
#endif
}