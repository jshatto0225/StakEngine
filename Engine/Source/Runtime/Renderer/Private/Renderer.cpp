#include "Renderer.h"

#include "RenderCommand.h"
#include "Renderer2D.h"
#include "Log.h"

namespace sk {
static bool running;

void renderer_init(const Unique<Window> &window) {
  SK_CORE_TRACE("Render command init");
  render_command_init(window);
  SK_CORE_TRACE("Renderer 2d init");
  renderer_2d_init();
  running = true;
}

void renderer_shutdown() {
  if (running) {
    running = false;
    renderer_2d_shutdown();
    render_command_shutdown();
  }
}
} // namespace SK
