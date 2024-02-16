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
Unique<Window> new_window(const std::string &name, i32 x, i32 y,
                                  i32 width, i32 height);
Unique<InputManager> new_input_manager();
Shared<RendererAPI> new_renderer_api(const Unique<Window> &window);
Shared<VertexArray> new_vertex_array();
Shared<VertexBuffer> new_vertex_buffer(u32 size);
Shared<IndexBuffer> new_index_buffer(u32 *indices, u32 size);
Shared<Shader> new_shader(const std::string &vs, const std::string &fs);
Shared<Texture> new_texture(TextureSpecification texture_spec);
Shared<Texture2D> new_texture_2d(TextureSpecification texture_spec);

#ifdef SK_OPENGL
Unique<GLContext> new_context(const Unique<Window> &window);
#endif
} // namespace SK
