#pragma once

#include "Types.h"

namespace sk {
enum ShaderDataType {

};

struct BufferElement {
  const char *name;
  ShaderDataType type;
  u32 size;
  u64 offset;
  bool normalized;
};

struct BufferLayout {
  BufferElement *elements;
  i32 num_elements;
  u32 stride;
};

class VertexBuffer {

};

class IndexBuffer {

};

class VertexArray {

};

class Shader {

};

class Texture {

};

namespace RenderApi {
void init();
void shutdown();

void set_clear_color(f32 r, f32 g, f32 b, f32 a);
// NOTE: x and y are relative to the window. (0, 0) is 
// always the bottom left
void set_viewport(i32 x, i32 y, i32 width, i32 height);
void set_line_width(f32 width);

void bind();
void clear();

void draw_indexed(const VertexArray &vao, u32 count);
void draw_lines(const VertexArray &vao, u32 count);
};
} // namespace sk