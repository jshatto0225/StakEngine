#include "RenderCommand.h"

#include "PlatformManager.h"
#include "RendererAPI.h"

namespace sk {
static Shared<RendererAPI> renderer_api;

void render_command_init(const Unique<Window> &window) {
  renderer_api = new_renderer_api(window);
}

void render_command_shutdown() {
  //renderer_api.reset();
}

void render_command_set_viewport(const Vec4u &viewport) {
  renderer_api->set_viewport(viewport);
}

void render_command_set_clear_color(const Vec4f &color) {
  renderer_api->set_clear_color(color);
}

void render_command_clear() {
  renderer_api->clear();
}

void render_command_draw_indexed(Shared<VertexArray> vao, u32 count) {
  renderer_api->draw_indexed(vao, count);
}

void render_command_draw_lines(Shared<VertexArray> vao, u32 count) {
  renderer_api->draw_lines(vao, count);
}

void render_command_set_line_width(f32 width) {
  renderer_api->set_line_width(width);
}
} // namespace SK
