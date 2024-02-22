#include "PlatformManager.h"

#include "Renderer.h"

#if defined(SK_WINDOWS)
#include "Win32Window.h"
#define PLATFORM_WINDOW Win32Window
#include "Win32InputManager.h"
#define PLATFORM_INPUT_MANAGER Win32InputManager
#elif defined(SK_LINUX)
#include "LinuxWindow.h"
#define PLATFORM_WINDOW LinuxWindow
#include "LinuxInputManager.h"
#define PLATFORM_INPUT_MANAGER LinuxInputManager
#else
#error Platform Not Supported
#endif

#if defined(SK_OPENGL)
#if defined(SK_WINDOWS)
#include "Win32GLContext.h"
#define PLATFORM_GL_CONTEXT Win32GLContext
#elif defined(SK_LINUX)
#include "LinuxGLContext.h"
#define PLATFORM_GL_CONTEXT LinuxGLContext
#endif
#include "OpenGLRendererAPI.h"
#define PLATFORM_RENDERER_API OpenGLRendererAPI
#include "OpenGLTexture.h"
#define PLATFORM_TEXTURE OpenGLTexture
#define PLATFORM_TEXTURE_2D OpenGLTexture2D
#include "OpenGLBuffer.h"
#define PLATFORM_VERTEX_BUFFER OpenGLVertexBuffer
#define PLATFORM_INDEX_BUFFER OpenGLIndexBuffer
#include "OpenGLVertexArray.h"
#define PLATFORM_VERTEX_ARRAY OpenGLVertexArray
#include "OpenGLShader.h"
#define PLATFORM_SHADER OpenGLShader
#elif defined(SK_VULKAN)
#include "VulkanRendererAPI.h"
#define PLATFORM_RENDERER_API VulkanRendererAPI
#include "VulkanTexture.h"
#define PLATFORM_TEXTURE VulkanTexture
#define PLATFORM_TEXTURE_2D VulkanTexture2D
#include "VulkanBuffer.h"
#define PLATFORM_VERTEX_BUFFER VulkanVertexBuffer
#define PLATFORM_INDEX_BUFFER VulkanIndexBuffer
#include "VulkanVertexArray.h"
#define PLATFORM_VERTEX_ARRAY VulkanVertexArray
#include "Shader.h"
#define PLATFORM_SHADER VulkanShader
#else
#error Platform Not Supported
#endif

Window *sk_create_window(const std::string &name, i32 x, i32 y, i32 width, i32 height) {
  return new PLATFORM_WINDOW(name, x, y, width, height);
}

InputManager *sk_create_input_manager() {
  return new PLATFORM_INPUT_MANAGER();
}

RendererAPI *sk_create_renderer_api(Window *window) {
  return new PLATFORM_RENDERER_API(window);
}

Texture *sk_create_texture(const TextureSpecification &texture_spec) {
  return new PLATFORM_TEXTURE(texture_spec);
}

Texture2D *sk_create_texture_2d(const TextureSpecification &texture_spec) {
  return new PLATFORM_TEXTURE_2D(texture_spec);
}

VertexBuffer *sk_create_vertex_buffer(u32 size) {
  return new PLATFORM_VERTEX_BUFFER(size);
}

IndexBuffer *sk_create_index_buffer(u32 *indices, u32 size) {
  return new PLATFORM_INDEX_BUFFER(indices, size);
}

VertexArray *sk_create_vertex_array() {
  return new PLATFORM_VERTEX_ARRAY();
}

Shader *sk_create_shader(const std::string &vs, const std::string &fs) {
  return new PLATFORM_SHADER(vs, fs);
}

#ifdef SK_OPENGL
GLContext *sk_create_context(Window *window) {
  return new PLATFORM_GL_CONTEXT(window);
}
#endif

