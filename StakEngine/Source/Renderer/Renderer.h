#pragma once

#include <imgui.h>

#include "RHICommandContext.h"
#include "RHIDevice.h"
#include "RHITexture.h"
#include "Window.h"

class FRenderProxy {
public:
    virtual ~FRenderProxy() = default;

    virtual void Render(FRHICommandContext &CommandContext) {}
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
    FRHICommandContext CommandContext;
    FRHIDevice Device;
    TRef<IWindow> Window;
    FRHITexture Backbuffer;

    FRHIPipelineLayout PipelineLayout;
    FRHIPipeline Pipeline;
};
