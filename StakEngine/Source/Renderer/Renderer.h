#pragma once

#include <imgui.h>

#include "RHICommandContext.h"
#include "RHITexture.h"
#include "Window.h"

class FRenderProxy {
public:
    virtual ~FRenderProxy() = default;

    virtual void Render(TRef<IRHICommandContext> CommandContext) {}
};

class FRenderer {
public:
    FRenderer() = default;

public:
    bool Init(TRef<IWindow> Window);
    void Shutdown();

    void InitImGui();
    void ImGuiNewFrame();
    void ShutdownImGui();
    bool Render();

    void AddProxy(FRenderProxy *Proxy);
    void RemoveProxy(FRenderProxy *Proxy);

    void SetPostProxy(FRenderProxy *Proxy);
    void UnsetPostProxy();

    FRenderer(FRenderer &) = delete;
    FRenderer &operator=(FRenderer &) = delete;

private:
    std::vector<FRenderProxy *> RenderProxies;
    FRenderProxy *PostRenderProxy = nullptr;
    TRef<IRHICommandContext> CommandContext;
    TRef<IWindow> Window = nullptr;
    TRef<IRHITexture> Backbuffer = nullptr;

    TRef<IRHIPipelineLayout> PipelineLayout = nullptr;
    TRef<IRHIPipeline> Pipeline = nullptr;
};
