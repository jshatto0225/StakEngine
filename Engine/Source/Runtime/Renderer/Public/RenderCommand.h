#pragma once

#include "RendererAPI.h"
#include "VertexArray.h"
#include "Window.h"

namespace sk {
  void render_command_init(const Unique<Window> &window);
  void render_command_shutdown();
  void render_command_set_viewport(const Vec4u &viewport);
  void render_command_set_clear_color(const Vec4f &color);
  void render_command_clear();
  void render_command_draw_indexed(Shared<VertexArray> vao, u32 count);
  void render_command_draw_lines(Shared<VertexArray> vao, u32 count);
  void render_command_set_line_width(f32 width);
} // namespace sk
