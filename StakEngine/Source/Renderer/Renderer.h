#pragma once

#include <imgui.h>

#include "RHICommandContext.h"
#include "RHITexture.h"
#include "Window.h"

using FRenderFunc = std::function<void(TRef<IRHICommandContext>)>;

struct FRenderer {
    static const FUInt32 MaxFramesInFlight = 2;

    FRenderFunc GuiRenderFunc;

    TRef<IRHICommandContext> CommandContext;
    FWindow *Window;
    TRef<IRHITexture> SwapchainBackbuffer;

    FUInt32 OffscreenBackbufferImageIndex;
    TRef<IRHITexture> OffscreenBackbuffers[MaxFramesInFlight];
    bool UseOffscreenBuffer;

    TRef<IRHIPipelineLayout> PipelineLayout;
    TRef<IRHIPipeline> Pipeline;
};

bool RendererInit(FRenderer *Renderer, FWindow *Window, bool RenderToOffscreenBuffer);

void RendererShutdown(FRenderer *Renderer);

bool RendererInitImGui(FRenderer *Renderer);

void RendererImGuiNewFrame(FRenderer *Renderer);

void RendererShutdownImGui(FRenderer *Renderer);

bool RendererRender(FRenderer *Renderer);

void RendererAddSceneToImGuiWindow(FRenderer *Renderer);