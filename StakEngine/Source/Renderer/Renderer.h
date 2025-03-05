#pragma once

#include <imgui.h>

#include "RHICommandContext.h"
#include "RHIDevice.h"
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
    void Init(TRef<IWindow> Window);
    void Shutdown();

    void InitImGui();
    void ImGuiNewFrame();
    void ShutdownImGui();
    void Render();

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
    TRef<IRHIDevice> Device;
    TRef<IWindow> Window;
    TRef<IRHITexture> Backbuffer;

    TRef<IRHIPipelineLayout> PipelineLayout;
    TRef<IRHIPipeline> Pipeline;
};
