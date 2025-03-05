#pragma once

#include "RHIDescriptorSetLayout.h"
#include "RHIPipeline.h"
#include "Window.h"

#include "RHIForward.h"

class IRHIDevice {
public:
    virtual ~IRHIDevice() = default;

    virtual void PrepareFrame() = 0;
    virtual void PresentFrame() = 0;

    virtual void ImGuiNewFrame() = 0;
    virtual void ShutdownImGui() = 0;
    virtual void InitImGui() = 0;

    virtual void WaitForGPUIdle() = 0;

    virtual FUInt32 GetGraphicsQueueIndex() = 0;
    virtual FUInt32 GetPresentQueueIndex() = 0;

    virtual void Submit(TRef<IRHICommandContext> Context) = 0;

    virtual TRef<IRHICommandContext> CreateCommandContext() = 0;

    virtual TRef<IRHIShader> CreateShader(const FRHIShaderDescription &Description) = 0;

    virtual TRef<IRHIBuffer> CreateBuffer(const FRHIBufferDescription &Description) = 0;

    virtual TRef<IRHIPipelineLayout> CreatePipelineLayout(const FRHIPipelineLayoutDescription &Description) = 0;

    virtual TRef<IRHIDescriptorSetLayout> CreateDescriptorSetLayout(const FRHIDescriptorSetLayoutDescription &Description) = 0;

    virtual TRef<IRHIPipeline> CreatePipeline(const FRHIGraphicsPipelineStateDescription &Description) = 0;

    virtual TRef<IRHITexture> GetBackbuffer() = 0;

    virtual void Shutdown() = 0;
};
