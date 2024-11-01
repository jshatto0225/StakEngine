#include "RendererAPI.h"

#if defined(SK_WINDOWS) or defined(SK_LINUX)
#include "../Platform/Vulkan/VulkanRendererAPI.h"
#endif

namespace Stak {

Scope<RendererAPI> RendererAPI::Create(Ref<Window> window) {
#if defined(SK_WINDOWS) defined(SK_LINUX)
  return CreateScope<VulkanRendererAPI>(window);
#else
  SK_LOG_CRITICAL("Invalid Render Platform");
  return NULL;
#endif
}

} // namespace Stak