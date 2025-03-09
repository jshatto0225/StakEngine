#pragma once

#include "Types.h"
#include "RHIForward.h"
#include "Window.h"

#include <imgui.h>

class IRHI {
public:
    virtual bool Init() = 0;

    virtual void Shutdown() = 0;

    virtual void ImGuiNewFrame() = 0;

    virtual void ShutdownImGui() = 0;

    virtual void InitImGui() = 0;

    virtual bool WaitForGPUIdle() = 0;

    virtual bool Submit(TRef<IRHICommandContext> Context) = 0;

    virtual TRef<IRHICommandContext> CreateCommandContext() = 0;

    virtual TRef<IRHIShader> CreateShader() = 0;

    virtual TRef<IRHIBuffer> CreateBuffer() = 0;

    virtual TRef<IRHIPipelineLayout> CreatePipelineLayout() = 0;

    virtual TRef<IRHIDescriptorSetLayout> CreateDescriptorSetLayout() = 0;

    virtual TRef<IRHIPipeline> CreatePipeline() = 0;

    virtual bool PrepareFrame() = 0;

    virtual bool PresentFrame() = 0;

    virtual bool SetActiveViewport(TRef<IRHIViewport> Viewport) = 0;

    virtual TRef<IRHITexture> GetCurrentBackbuffer() = 0;

    virtual TRef<IRHIViewport> CreateViewport(void *WindowHandle) = 0;

public:
    virtual ~IRHI() = default;
};

bool RHIInit();

void RHIShutdown();

TRef<IRHIViewport> RHICreateViewport(void *WindowHandle);

TRef<IRHITexture> RHIGetCurrentBackbuffer();

bool RHISetActiveViewport(TRef<IRHIViewport> Viewport);

void RHIImGuiNewFrame();

void RHIShutdownImGui();

void RHIInitImGui();

bool RHIWaitForGPUIdle();

bool RHISubmit(TRef<IRHICommandContext> Context);

TRef<IRHICommandContext> RHICreateCommandContext();

TRef<IRHIShader> RHICreateShader();

TRef<IRHIBuffer> RHICreateBuffer();

TRef<IRHIPipelineLayout> RHICreatePipelineLayout();

TRef<IRHIDescriptorSetLayout> RHICreateDescriptorSetLayout();

TRef<IRHIPipeline> RHICreatePipeline();

bool RHIPrepareFrame();

bool RHIPresentFrame();
