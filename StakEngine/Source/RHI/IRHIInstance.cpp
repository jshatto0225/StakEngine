#include "IRHIInstance.h"

#include "FVulkanRHIInstance.h"

TScope<IRHIInstance> IRHIInstance::Create() {
#ifdef SK_VULKAN
  return TCreateScope<FVulkanRHIInstance>();
#endif
}
