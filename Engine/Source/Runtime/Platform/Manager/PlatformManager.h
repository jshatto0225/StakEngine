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

Window *sk_create_window(const std::string &name, i32 x, i32 y, i32 width, i32 height);
InputManager *sk_create_input_manager();
RendererAPI *sk_create_renderer_api(Window *window);
VertexArray *sk_create_vertex_array();
VertexBuffer *sk_create_vertex_buffer(u32 size);
IndexBuffer *sk_create_index_buffer(u32 *indices, u32 size);
Shader *sk_create_shader(const std::string &vs, const std::string &fs);
Texture *sk_create_texture(const TextureSpecification &texture_spec);
Texture2D *sk_create_texture_2d(const TextureSpecification &texture_spec);

#ifdef SK_OPENGL
GLContext *sk_create_context(Window *window);
#endif

