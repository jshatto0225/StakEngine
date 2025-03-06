#include "RHI.h"

#include "Asserts.h"

#include "RHIViewport.h"

#ifdef SK_VULKAN
#include "VulkanRHI.h"
using FPlatformRHI = FVulkanRHI;
#endif

IRHI *GRHI;

void RHIInit() {
    GRHI = new FPlatformRHI();
}

void RHIShutdown() {
    if (GRHI) {
        delete GRHI;
        GRHI = nullptr;
    }
}

void RHIImGuiNewFrame() {
    assert(GRHI);
    GRHI->ImGuiNewFrame();
}

void RHIShutdownImGui() {
    assert(GRHI);
    GRHI->ShutdownImGui();
}

void RHIInitImGui() {
    assert(GRHI);
    GRHI->InitImGui();
}

void RHIWaitForGPUIdle() {
    assert(GRHI);
    GRHI->WaitForGPUIdle();
}

void RHISubmit(TRef<IRHICommandContext> Context) {
    assert(GRHI);
    GRHI->Submit(Context);
}

TRef<IRHICommandContext> RHICreateCommandContext() {
    assert(GRHI);
    return GRHI->CreateCommandContext();
}

TRef<IRHIShader> RHICreateShader(const FRHIShaderDescription &Description) {
    assert(GRHI);
    return GRHI->CreateShader(Description);
}

TRef<IRHIBuffer> RHICreateBuffer(const FRHIBufferDescription &Description) {
    assert(GRHI);
    return GRHI->CreateBuffer(Description);
}

TRef<IRHIPipelineLayout> RHICreatePipelineLayout(const FRHIPipelineLayoutDescription &Description) {
    assert(GRHI);
    return GRHI->CreatePipelineLayout(Description);
}

TRef<IRHIDescriptorSetLayout> RHICreateDescriptorSetLayout(const FRHIDescriptorSetLayoutDescription &Description) {
    assert(GRHI);
    return GRHI->CreateDescriptorSetLayout(Description);
}

TRef<IRHIPipeline> RHICreatePipeline(const FRHIGraphicsPipelineStateDescription &Description) {
    assert(GRHI);
    return GRHI->CreatePipeline(Description);
}

TRef<IRHITexture> RHIGetCurrentBackbuffer() {
    assert(GRHI);
    return GRHI->GetCurrentBackbuffer();
}

void RHISetActiveViewport(TRef<IRHIViewport> Viewport) {
    assert(GRHI);
    GRHI->SetActiveViewport(Viewport);
}

void RHIPrepareFrame() {
    assert(GRHI);
    GRHI->PrepareFrame();
}

void RHIPresentFrame() {
    assert(GRHI);
    GRHI->PresentFrame();
}

TRef<IRHIViewport> RHICreateViewport(void *WindowHandle) {
    assert(GRHI);
    return GRHI->CreateViewport(WindowHandle);
}