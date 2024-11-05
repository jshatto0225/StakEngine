#include "RendererAPI.h"

#if defined(SK_WINDOWS) or defined(SK_LINUX)
#include "VulkanRendererAPI.h"
#endif

namespace Stak {

Ref<RendererAPI> RendererAPI::Create(Ref<Window> window, std::string appName) {
#if defined(SK_WINDOWS) or defined(SK_LINUX)
  return CreateRef<VulkanRendererAPI>(window, appName);
#else
  SK_LOG_CRITICAL("Invalid Render Platform");
  return NULL;
#endif
}

} // namespace Stak