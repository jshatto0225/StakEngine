#pragma once

#include "RendererAPI.h"
#include "VertexArray.h"
#include "Window.h"

namespace sk {
class RenderCommand final {
public:
  static void init(const Unique<Window> &window);
  static void shutdown();
  static void set_viewport(const Vec4u &viewport);
  static void set_clear_color(const Vec4f &color);
  static void clear();
  static void draw_indexed(Shared<VertexArray> vao, u32 count);
  static void draw_lines(Shared<VertexArray> vao, u32 count);
  static void set_line_width(f32 width);

private:
  static Shared<RendererAPI> renderer_api;
};
} // namespace SK
