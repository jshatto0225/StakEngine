#pragma once

#include <imgui.h>

#include "RHI.h"
#include "Window.h"

using FRenderFunc = std::function<void(FRHIResourceHandle)>;

struct FRenderer {
    static const FUInt32 MaxFramesInFlight = 2;

    FRenderFunc GuiRenderFunc;

    FRHIResourceHandle CommandList;
    FWindow *Window;
    FRHIResourceHandle SwapchainTexture;

    FUInt32 OffscreenBackbufferImageIndex;
    FRHIResourceHandle  OffscreenBackbuffers[MaxFramesInFlight];
    bool UseOffscreenBuffer;

    FRHIResourceHandle PipelineLayout;
    FRHIResourceHandle Pipeline;
};

bool RendererInit(FRenderer *Renderer, FWindow *Window, bool RenderToOffscreenBuffer);

void RendererShutdown(FRenderer *Renderer);

bool RendererInitImGui(FRenderer *Renderer);

void RendererImGuiNewFrame(FRenderer *Renderer);

void RendererShutdownImGui(FRenderer *Renderer);

bool RendererRender(FRenderer *Renderer);

void RendererAddSceneToImGuiWindow(FRenderer *Renderer);