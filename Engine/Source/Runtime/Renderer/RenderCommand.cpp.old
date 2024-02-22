#include "RenderCommand.h"

#include "PlatformManager.h"
#include "RendererAPI.h"

static RendererAPI *renderer_api;

void sk_render_command_init(Window *window) {
  renderer_api = sk_create_renderer_api(window);
}

void sk_render_command_shutdown() {
  delete renderer_api;
}

void sk_render_command_set_viewport(const Vec4u &viewport) {
  renderer_api->set_viewport(viewport);
}

void sk_render_command_set_clear_color(const Vec4f &color) {
  renderer_api->set_clear_color(color);
}

void sk_render_command_clear() {
  renderer_api->clear();
}

void sk_render_command_draw_indexed(VertexArray *vao, u32 count) {
  renderer_api->draw_indexed(vao, count);
}

void sk_render_command_draw_lines(VertexArray *vao, u32 count) {
  renderer_api->draw_lines(vao, count);
}

void sk_render_command_set_line_width(f32 width) {
  renderer_api->set_line_width(width);
}

