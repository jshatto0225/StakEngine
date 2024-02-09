#pragma once

#include "InputManager.h"
#include "RendererAPI.h"
#include "Texture.h"
#include "Types.h"
#include "Window.h"
#include "Shader.h"

#ifdef SK_OPENGL
#include "GLContext.h"
#endif

namespace sk {
class PlatformManager final {
public:
  static Unique<Window> new_window(const std::string &name, i32 x, i32 y,
                                  i32 width, i32 height);
  static Unique<InputManager> new_input_manager();
  static Shared<RendererAPI> new_renderer_api(const Unique<Window> &window);
  static Shared<VertexArray> new_vertex_array();
  static Shared<VertexBuffer> new_vertex_buffer(u32 size);
  static Shared<IndexBuffer> new_index_buffer(u32 *indices, u32 size);
  static Shared<Shader> new_shader(const std::string &vs, const std::string &fs);
  static Shared<Texture> new_texture(TextureSpecification texture_spec);
  static Shared<Texture2D> new_texture_2d(TextureSpecification texture_spec);
  static void kill_platform_window_manager();

#ifdef SK_OPENGL
  static Unique<GLContext> new_context(const Unique<Window> &window);
#endif
};
} // namespace SK
