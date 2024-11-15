#include "IRHIDevice.h"

#ifdef SK_VULKAN
#include "FVulkanRHIDevice.h"
typedef FVulkanRHIInstance FPlatformRHIInstance;
typedef FVulkanRHIDevice FPlatformRHIDevices;
#endif

TRef<IRHIDevice> IRHIDevice::Create(TRef<IRHIInstance> Instance, TRef<IWindow> Window) {
  return TCreateRef<FPlatformRHIDevices>(std::static_pointer_cast<FPlatformRHIInstance>(Instance), Window);
}