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
    bool Init(TRef<IWindow> Window, bool RenderToOffscreenBuffer);
    void Shutdown();

    bool InitImGui();
    void ImGuiNewFrame();
    void ShutdownImGui();
    bool Render();

    void AddProxy(FRenderProxy *Proxy);
    void RemoveProxy(FRenderProxy *Proxy);

    void SetPostProxy(FRenderProxy *Proxy);
    void UnsetPostProxy();

    void AddSceneToImGuiWindow();

    FRenderer(FRenderer &) = delete;
    FRenderer &operator=(FRenderer &) = delete;

private:
    std::vector<FRenderProxy *> RenderProxies;
    FRenderProxy *PostRenderProxy = nullptr;
    TRef<IRHICommandContext> CommandContext;
    TRef<IWindow> Window = nullptr;
    TRef<IRHITexture> SwapchainBackbuffer;

    FUInt32 OffscreenBackbufferImageIndex = 0;
    static const FUInt32 MaxFramesInFlight = 2;
    TRef<IRHITexture> OffscreenBackbuffers[MaxFramesInFlight];
    bool UseOffscreenBuffer = false;

    TRef<IRHIPipelineLayout> PipelineLayout = nullptr;
    TRef<IRHIPipeline> Pipeline = nullptr;
};
