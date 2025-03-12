#pragma once

#include <imgui.h>

#include "RHICommandContext.h"
#include "RHITexture.h"
#include "Window.h"

using FRenderFunc = std::function<void(TRef<IRHICommandContext>)>;

struct FRenderer {
    bool Init(FWindow *Window, bool RenderToOffscreenBuffer);
    void Shutdown();

    bool InitImGui();
    void ImGuiNewFrame();
    void ShutdownImGui();
    bool Render();

    void AddSceneToImGuiWindow();

    FRenderFunc GuiRenderFunc;

    TRef<IRHICommandContext> CommandContext;
    FWindow *Window = nullptr;
    TRef<IRHITexture> SwapchainBackbuffer;

    FUInt32 OffscreenBackbufferImageIndex = 0;
    static const FUInt32 MaxFramesInFlight = 2;
    TRef<IRHITexture> OffscreenBackbuffers[MaxFramesInFlight];
    bool UseOffscreenBuffer = false;

    TRef<IRHIPipelineLayout> PipelineLayout = nullptr;
    TRef<IRHIPipeline> Pipeline = nullptr;
};
