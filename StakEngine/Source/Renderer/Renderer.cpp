#include "Renderer.h"

namespace Stak {

Ref<RendererAPI> Renderer::s_API = NULL;

void Renderer::Init(Ref<Window> window) {
  s_API = RendererAPI::Create(window, "APP_NAME");
}

void Renderer::Shutdown() {
  s_API->WaitForDevice();
}

} // namespace Stak
