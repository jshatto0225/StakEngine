#pragma once

#include "Types.h"
#include "Window.h"
#include "InputManager.h"
#include "RendererAPI.h"
#include "Texture.h"


class PlatformManager final
{
public:
    static Shared<Window> NewWindow(const std::string& name, i32 x, i32 y, i32 width, i32 height);
    static Unique<InputManager> NewInputManager();
    static Shared<RendererAPI> NewRendererAPI();
    static Shared<VertexArray> NewVertexArray();
    static Shared<VertexBuffer> NewVertexBuffer(u32 size);
    static Shared<IndexBuffer> NewIndexBuffer(u32* indices, u32 size);
    static Shared<Shader> NewShader(const std::string& vs, const std::string& fs);
    static Shared<Texture> NewTexture(TextureSpecification textureSpecification);
    static Shared<Texture2D> NewTexture2D(TextureSpecification textureSpecification);
};
