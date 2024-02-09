#include "RenderCommand.h"

#include "PlatformManager.h"
#include "RendererAPI.h"

namespace sk {
Shared<RendererAPI> RenderCommand::renderer_api;

void RenderCommand::init(const Unique<Window> &window) {
  renderer_api = PlatformManager::new_renderer_api(window);
}

void RenderCommand::shutdown() { renderer_api.reset(); }

void RenderCommand::set_viewport(const Vec4u &viewport) {
  renderer_api->set_viewport(viewport);
}

void RenderCommand::set_clear_color(const Vec4f &color) {
  renderer_api->set_clear_color(color);
}

void RenderCommand::clear() { renderer_api->clear(); }

void RenderCommand::draw_indexed(Shared<VertexArray> vao, u32 count) {
  renderer_api->draw_indexed(vao, count);
}

void RenderCommand::draw_lines(Shared<VertexArray> vao, u32 count) {
  renderer_api->draw_lines(vao, count);
}

void RenderCommand::set_line_width(f32 width) {
  renderer_api->set_line_width(width);
}
} // namespace SK
