#pragma once

#include "Types.h"
#include "Vec4Types.h"
#include "VertexArray.h"

namespace sk {
class RendererAPI {
public:
  RendererAPI() = default;
  virtual ~RendererAPI() {}

  virtual void draw_indexed(Shared<VertexArray> vao, u32 count) = 0;
  virtual void draw_lines(Shared<VertexArray> vao, u32 count) = 0;
  virtual void set_line_width(f32 width) = 0;
  virtual void clear() = 0;
  virtual void set_clear_color(const Vec4f &color) = 0;
  virtual void set_viewport(const Vec4u &viewport) = 0;
};
} // namespace SK
