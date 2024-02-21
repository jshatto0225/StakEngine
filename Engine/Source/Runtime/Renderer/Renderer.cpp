#include "Renderer.h"

#include "RenderCommand.h"
#include "Renderer2D.h"
#include "Log.h"

static bool running;

void sk_renderer_init(Window *window) {
  SK_CORE_TRACE("Render command init");
  sk_render_command_init(window);
  SK_CORE_TRACE("Renderer 2d init");
  sk_renderer_2d_init();
  running = true;
}

void sk_renderer_shutdown() {
  if (running) {
    running = false;
    sk_renderer_2d_shutdown();
    sk_render_command_shutdown();
  }
}

