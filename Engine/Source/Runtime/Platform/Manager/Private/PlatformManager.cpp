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

namespace sk {
Unique<Window> new_window(const std::string &name, i32 x, i32 y, i32 width, i32 height) {
  return new_unique<PLATFORM_WINDOW>(name, x, y, width, height);
}

Unique<InputManager> new_input_manager() {
  return new_unique<PLATFORM_INPUT_MANAGER>();
}

Shared<RendererAPI> new_renderer_api(const Unique<Window> &window) {
  return new_shared<PLATFORM_RENDERER_API>(window);
}

Shared<Texture> new_texture(TextureSpecification texture_spec) {
  return new_shared<PLATFORM_TEXTURE>(texture_spec);
}

Shared<Texture2D> new_texture_2d(TextureSpecification texture_spec) {
  return new_shared<PLATFORM_TEXTURE_2D>(texture_spec);
}

Shared<VertexBuffer> new_vertex_buffer(u32 size) {
  return new_shared<PLATFORM_VERTEX_BUFFER>(size);
}

Shared<IndexBuffer> new_index_buffer(u32 *indices, u32 size) {
  return new_shared<PLATFORM_INDEX_BUFFER>(indices, size);
}

Shared<VertexArray> new_vertex_array() {
  return new_shared<PLATFORM_VERTEX_ARRAY>();
}

Shared<Shader> new_shader(const std::string &vs, const std::string &fs) {
  return new_shared<PLATFORM_SHADER>(vs, fs);
}

#ifdef SK_OPENGL
Unique<GLContext> new_context(const Unique<Window> &window) {
  return new_unique<PLATFORM_GL_CONTEXT>(window);
}
#endif
} // namespace SK
