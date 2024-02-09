#include "Renderer.h"

#include "RenderCommand.h"
#include "Renderer2D.h"

namespace sk {
bool Renderer::running;

void Renderer::init(const Unique<Window> &window) {
  RenderCommand::init(window);
  Renderer2D::init();
  running = true;
}

void Renderer::shutdown() {
  if (running) {
    running = false;
    Renderer2D::shutdown();
    RenderCommand::shutdown();
  }
}
} // namespace SK
