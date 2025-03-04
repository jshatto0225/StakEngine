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

    virtual void Submit(FRHICommandContext &Context) = 0;

    virtual TRef<IRHICommandContext> CreateCommandContext() = 0;

    virtual TRef<IRHIShader> CreateShader(const FRHIShaderDescription &Description) = 0;

    virtual TRef<IRHIBuffer> CreateBuffer(const FRHIBufferDescription &Description) = 0;

    virtual TRef<IRHIPipelineLayout> CreatePipelineLayout(const FRHIPipelineLayoutDescription &Description) = 0;

    virtual TRef<IRHIDescriptorSetLayout> CreateDescriptorSetLayout(const FRHIDescriptorSetLayoutDescription &Description) = 0;

    virtual TRef<IRHIPipeline> CreatePipeline(const FRHIGraphicsPipelineStateDescription &Description) = 0;

    virtual FRHITexture &GetBackbuffer() = 0;

    virtual void Shutdown() = 0;
};

class FRHIDevice {
public:
    FRHIDevice() = default;

public:
    FRHIDevice(TRef<IWindow> Window);

    void PrepareFrame();
    void PresentFrame();

    void ImGuiNewFrame();
    void ShutdownImGui();
    void InitImGui();

    void WaitForGPUIdle();

    FUInt32 GetGraphicsQueueIndex();
    FUInt32 GetPresentQueueIndex();

    void Submit(FRHICommandContext& Context);

    FRHICommandContext CreateCommandContext();

    FRHIBuffer CreateBuffer(const FRHIBufferDescription &Description);

    FRHIShader CreateShader(const FRHIShaderDescription &Description);

    FRHIPipelineLayout CreatePipelineLayout(const FRHIPipelineLayoutDescription &Description);

    FRHIDescriptorSetLayout CreateDescriptorSetLayout(const FRHIDescriptorSetLayoutDescription &Description);

    FRHIPipeline CreatePipeline(const FRHIGraphicsPipelineStateDescription &Description);

    FRHITexture &GetBackbuffer();

    void Shutdown();

private:
    TRef<IRHIDevice> Impl = nullptr;
};
