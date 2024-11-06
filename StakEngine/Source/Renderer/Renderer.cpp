#include "Renderer.h"

namespace Stak {

Ref<RendererAPI> Renderer::s_API = NULL;

void Renderer::init(Ref<Window> window) {
  s_API = RendererAPI::create(window, "APP_NAME");
}

void Renderer::shutdown() {
}

} // namespace Stak
