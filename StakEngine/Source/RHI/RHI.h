#pragma once

#include "Types.h"
#include "RHIForward.h"
#include "Window.h"

#include <imgui.h>

class IRHI {
public:
    IRHI() = default;

    IRHI(IRHI &) = delete;
    IRHI &operator=(IRHI &) = delete;

public:
    virtual void ImGuiNewFrame() = 0;

    virtual void ShutdownImGui() = 0;

    virtual void InitImGui() = 0;

    virtual void WaitForGPUIdle() = 0;

    virtual void Submit(TRef<IRHICommandContext> Context) = 0;

    virtual TRef<IRHICommandContext> CreateCommandContext() = 0;

    virtual TRef<IRHIShader> CreateShader(const FRHIShaderDescription &Description) = 0;

    virtual TRef<IRHIBuffer> CreateBuffer(const FRHIBufferDescription &Description) = 0;

    virtual TRef<IRHIPipelineLayout> CreatePipelineLayout(const FRHIPipelineLayoutDescription &Description) = 0;

    virtual TRef<IRHIDescriptorSetLayout> CreateDescriptorSetLayout(const FRHIDescriptorSetLayoutDescription &Description) = 0;

    virtual TRef<IRHIPipeline> CreatePipeline(const FRHIGraphicsPipelineStateDescription &Description) = 0;

    virtual void PrepareFrame() = 0;

    virtual void PresentFrame() = 0;

    virtual void SetActiveViewport(TRef<IRHIViewport> Viewport) = 0;

    virtual TRef<IRHITexture> GetCurrentBackbuffer() = 0;

    virtual TRef<IRHIViewport> CreateViewport(void *WindowHandle) = 0;
};

void RHIInit();

void RHIShutdown();

TRef<IRHIViewport> RHICreateViewport(void *WindowHandle);

TRef<IRHITexture> RHIGetCurrentBackbuffer();

void RHISetActiveViewport(TRef<IRHIViewport> Viewport);

void RHIImGuiNewFrame();

void RHIShutdownImGui();

void RHIInitImGui();

void RHIWaitForGPUIdle();

void RHISubmit(TRef<IRHICommandContext> Context);

TRef<IRHICommandContext> RHICreateCommandContext();

TRef<IRHIShader> RHICreateShader(const FRHIShaderDescription &Description);

TRef<IRHIBuffer> RHICreateBuffer(const FRHIBufferDescription &Description);

TRef<IRHIPipelineLayout> RHICreatePipelineLayout(const FRHIPipelineLayoutDescription &Description);

TRef<IRHIDescriptorSetLayout> RHICreateDescriptorSetLayout(const FRHIDescriptorSetLayoutDescription &Description);

TRef<IRHIPipeline> RHICreatePipeline(const FRHIGraphicsPipelineStateDescription &Description);

void RHIPrepareFrame();

void RHIPresentFrame();