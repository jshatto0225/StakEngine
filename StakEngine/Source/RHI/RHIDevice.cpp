#include "RHIDevice.h"

#include "RHITexture.h"
#include "RHICommandContext.h"
#include "RHIBuffer.h"
#include "RHIShader.h"
#include "RHI.h"

FRHIDevice::FRHIDevice(TRef<IWindow> Window) {
    Impl = RHICreateDeviceImpl(Window);
}

void FRHIDevice::Shutdown() {
    Impl->Shutdown();
    Impl = nullptr;
}

void FRHIDevice::PresentFrame() {
    Impl->PresentFrame();
}

void FRHIDevice::PrepareFrame() {
    Impl->PrepareFrame();
}

void FRHIDevice::Submit(FRHICommandContext &Context) {
    Impl->Submit(Context);
}

void FRHIDevice::WaitForGPUIdle() {
    Impl->WaitForGPUIdle();
}

void FRHIDevice::ImGuiNewFrame() {
    Impl->ImGuiNewFrame();
}

void FRHIDevice::ShutdownImGui() {
    Impl->ShutdownImGui();
}

void FRHIDevice::InitImGui() {
    Impl->InitImGui();
}

FRHICommandContext FRHIDevice::CreateCommandContext() {
    return Impl->CreateCommandContext();
}

FUInt32 FRHIDevice::GetGraphicsQueueIndex() {
    return Impl->GetGraphicsQueueIndex();
}

FUInt32 FRHIDevice::GetPresentQueueIndex() {
    return Impl->GetPresentQueueIndex();
}

FRHITexture &FRHIDevice::GetBackbuffer() {
    return Impl->GetBackbuffer();
}

FRHIBuffer FRHIDevice::CreateBuffer(const FRHIBufferDescription &Description) {
    return Impl->CreateBuffer(Description);
}

FRHIShader FRHIDevice::CreateShader(const FRHIShaderDescription &Description) {
    return Impl->CreateShader(Description);
}

FRHIPipelineLayout FRHIDevice::CreatePipelineLayout(const FRHIPipelineLayoutDescription &Description) {
    return Impl->CreatePipelineLayout(Description);
}

FRHIDescriptorSetLayout FRHIDevice::CreateDescriptorSetLayout(const FRHIDescriptorSetLayoutDescription &Description) {
    return Impl->CreateDescriptorSetLayout(Description);
}

FRHIPipeline FRHIDevice::CreatePipeline(const FRHIPipelineStateDescription &Description) {
    return Impl->CreatePipeline(Description);
}
