#pragma once

#include "RendererAPI.h"
#include "VertexArray.h"
#include "Window.h"

void sk_render_command_init(Window *window);
void sk_render_command_shutdown();
void sk_render_command_set_viewport(const Vec4u &viewport);
void sk_render_command_set_clear_color(const Vec4f &color);
void sk_render_command_clear();
void sk_render_command_draw_indexed(VertexArray *vao, u32 count);
void sk_render_command_draw_lines(VertexArray *vao, u32 count);
void sk_render_command_set_line_width(f32 width);
