#include "IRHIDevice.h"

#ifdef SK_VULKAN
#include "FVulkanRHIDevice.h"
using FPlatformRHIInstance = FVulkanRHIInstance;
using FPlatformRHIDevices = FVulkanRHIDevice;
#endif

TRef<IRHIDevice> IRHIDevice::Create(TRef<IRHIInstance> Instance, TRef<IWindow> Window) {
  return TCreateRef<FPlatformRHIDevices>(std::static_pointer_cast<FPlatformRHIInstance>(Instance), Window, MAX_FRAMES_IN_FLIGHT);
}