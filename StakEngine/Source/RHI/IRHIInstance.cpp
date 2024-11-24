#include "IRHIInstance.h"

#include "FVulkanRHIInstance.h"

TScope<IRHIInstance> IRHIInstance::Create(const std::string &AppName) {
#ifdef SK_VULKAN
  return TCreateScope<FVulkanRHIInstance>(AppName);
#endif
}
