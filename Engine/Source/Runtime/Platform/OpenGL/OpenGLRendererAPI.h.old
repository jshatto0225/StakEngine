#pragma once

#include "GLContext.h"
#include "RendererAPI.h"
#include "Types.h"
#include "Window.h"

class OpenGLRendererAPI : public RendererAPI {
public:
  OpenGLRendererAPI(Window *window);

  void draw_indexed(VertexArray *vao, u32 count) override;
  void draw_lines(VertexArray *vao, u32 count) override;
  void set_line_width(f32 width) override;
  void clear() override;
  void set_clear_color(const Vec4f &color) override;
  void set_viewport(const Vec4u &viewport) override;

private:
  GLContext *context;
};

