#include "PlatformManager.h"

#include "Renderer.h"

#ifdef SK_WINDOWS
#include "Win32Window.h"
#define PLATFORM_WINDOW Win32Window
#include "Win32InputManager.h"
#define PLATFORM_INPUT_MANAGER Win32InputManager
#else
#error Platform Not Supported
#endif

#ifdef SK_OPENGL
#ifdef SK_WINDOWS
#include "Win32GLContext.h"
#define PLATFORM_GL_CONTEXT Win32GLContext
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

Unique<Window> PlatformManager::NewWindow(const std::string& name, i32 x, i32 y, i32 width, i32 height)
{
    return MakeUnique<PLATFORM_WINDOW>(name, x, y, width, height);
}

Unique<InputManager> PlatformManager::NewInputManager()
{
    return MakeUnique<PLATFORM_INPUT_MANAGER>();
}

Shared<RendererAPI> PlatformManager::NewRendererAPI()
{
    return MakeShared<PLATFORM_RENDERER_API>();
}

Shared<Texture> PlatformManager::NewTexture(TextureSpecification textureSpecification)
{
    return MakeShared<PLATFORM_TEXTURE>(textureSpecification);
}

Shared<Texture2D> PlatformManager::NewTexture2D(TextureSpecification textureSpecification)
{
    return MakeShared<PLATFORM_TEXTURE_2D>(textureSpecification);
}

Shared<VertexBuffer> PlatformManager::NewVertexBuffer(u32 size)
{
    return MakeShared<PLATFORM_VERTEX_BUFFER>(size);
}

Shared<IndexBuffer> PlatformManager::NewIndexBuffer(u32* indices, u32 size)
{
    return MakeShared<PLATFORM_INDEX_BUFFER>(indices, size);
}

Shared<VertexArray> PlatformManager::NewVertexArray()
{
    return MakeShared<PLATFORM_VERTEX_ARRAY>();
}

Shared<Shader> PlatformManager::NewShader(const std::string& vs, const std::string& fs)
{
    return MakeShared<PLATFORM_SHADER>(vs, fs);
}

void KillPlatformWindowManager()
{
#ifdef WIN32
    PostQuitMessage(0);
#endif
}

#ifdef SK_OPENGL
Unique<GLContext> PlatformManager::NewContext(u64 window)
{
    return MakeUnique<PLATFORM_GL_CONTEXT>(window);
}
#endif
