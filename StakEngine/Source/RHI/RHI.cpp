#include "RHI.h"

#include "Asserts.h"
#include "RHIViewport.h"

#ifdef SK_VULKAN
#include "VulkanRHI.h"
using FPlatformRHI = FVulkanRHI;
#endif

IRHI *GRHI;

bool RHIInit() {
    GRHI = new FPlatformRHI;
    assert(GRHI);
    return GRHI->Init();
}

void RHIShutdown() {
    assert(GRHI);
    GRHI->Shutdown();
    delete GRHI;
}

TRef<IRHITexture> RHICreateTexture() {
    assert(GRHI);
    return GRHI->CreateTexture();
}

void RHIImGuiNewFrame() {
    assert(GRHI);
    return GRHI->ImGuiNewFrame();
}

void RHIShutdownImGui() {
    assert(GRHI);
    return GRHI->ShutdownImGui();
}

void RHIInitImGui() {
    assert(GRHI);
    return GRHI->InitImGui();
}

bool RHIWaitForGPUIdle() {
    assert(GRHI);
    return GRHI->WaitForGPUIdle();
}

bool RHISubmit(TRef<IRHICommandContext> Context) {
    assert(GRHI);
    return GRHI->Submit(Context);
}

TRef<IRHICommandContext> RHICreateCommandContext() {
    assert(GRHI);
    return GRHI->CreateCommandContext();
}

TRef<IRHIShader> RHICreateShader() {
    assert(GRHI);
    return GRHI->CreateShader();
}

TRef<IRHIBuffer> RHICreateBuffer() {
    assert(GRHI);
    return GRHI->CreateBuffer();
}

TRef<IRHIPipelineLayout> RHICreatePipelineLayout() {
    assert(GRHI);
    return GRHI->CreatePipelineLayout();
}

TRef<IRHIDescriptorSetLayout> RHICreateDescriptorSetLayout() {
    assert(GRHI);
    return GRHI->CreateDescriptorSetLayout();
}

TRef<IRHIPipeline> RHICreatePipeline() {
    assert(GRHI);
    return GRHI->CreatePipeline();
}

TRef<IRHITexture> RHIGetCurrentBackbuffer() {
    assert(GRHI);
    return GRHI->GetCurrentBackbuffer();
}

bool RHISetActiveViewport(TRef<IRHIViewport> Viewport) {
    assert(GRHI);
    return GRHI->SetActiveViewport(Viewport);
}

bool RHIPrepareFrame() {
    assert(GRHI);
    return GRHI->PrepareFrame();
}

bool RHIPresentFrame() {
    assert(GRHI);
    return GRHI->PresentFrame();
}

TRef<IRHIViewport> RHICreateViewport(void *WindowHandle) {
    assert(GRHI);
    return GRHI->CreateViewport(WindowHandle);
}
