#pragma once

#include "Types.h"
#include "Window.h"
#include "InputManager.h"
#include "RendererAPI.h"
#include "Texture.h"

#ifdef SK_OPENGL
#include "GLContext.h"
#endif

namespace SK
{
    class PlatformManager final
    {
    public:
        static Unique<Window> NewWindow(const std::string& name, i32 x, i32 y, i32 width, i32 height);
        static Unique<InputManager> NewInputManager();
        static Shared<RendererAPI> NewRendererAPI(const Unique<Window>& window);
        static Shared<VertexArray> NewVertexArray();
        static Shared<VertexBuffer> NewVertexBuffer(u32 size);
        static Shared<IndexBuffer> NewIndexBuffer(u32* indices, u32 size);
        static Shared<Shader> NewShader(const std::string& vs, const std::string& fs);
        static Shared<Texture> NewTexture(TextureSpecification textureSpecification);
        static Shared<Texture2D> NewTexture2D(TextureSpecification textureSpecification);
        static void KillPlatformWindowManager();

    #ifdef SK_OPENGL
        static Unique<GLContext> NewContext(const Unique<Window>& window);
    #endif
    };
}
