#pragma once

#include "Types.h"

namespace sk {
enum ShaderDataType {

};

struct _SKBufferElement {
  const char *name;
  ShaderDataType type;
  u32 size;
  u64 offset;
  bool normalized;
};

struct _SKBufferLayout {
  _SKBufferElement *elements;
  i32 num_elements;
  u32 stride;
};

struct _SKVertexBuffer {

};

struct _SKIndexBuffer {

};

// Render Commands
void sk_render_api_set_clear_color(f32, f32, f32, f32);
void sk_render_api_clear();
void sk_render_api_init();
void sk_render_api_set_viewport(i32, i32, i32, i32);
void sk_render_api_shutdown();

// Rendering Context Stuff
void sk_bind_renderer();
} // namespace sk