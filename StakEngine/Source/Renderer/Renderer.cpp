#include "Renderer.h"

namespace Stak {

Renderer::Renderer(Ref<Window> window) {
  m_API = RendererAPI::Create(window);
}

Renderer::~Renderer() {
  m_API->WaitForDevice();
}

} // namespace Stak