#include "RHICore.h"

#ifdef SK_VULKAN
#include "VulkanRHI.h"
#endif

namespace Stak {

Scope<IRHI> IRHI::create() {
#ifdef SK_VULKAN
  return createScope<VulkanRHI>();
#endif
}

} // namespace Stak