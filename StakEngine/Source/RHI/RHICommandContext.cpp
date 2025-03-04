#include "RHICommandContext.h"

#include "RHI.h"

#include "RHIDevice.h"

FRHICommandContext::FRHICommandContext(TRef<IRHICommandContext> Impl) : Impl(Impl) {}

void FRHICommandContext::Begin() {
    Impl->Begin();
}

void FRHICommandContext::End() {
    Impl->End();
}

void FRHICommandContext::RenderImGuiDrawData(ImDrawData* DrawData) {
    Impl->RenderImGuiDrawData(DrawData);
}

void FRHICommandContext::Shutdown() {
    Impl->Shutdown();
    Impl = nullptr;
}

void FRHICommandContext::ResourceBarrier(const FRHIResourceBarrier& Barrier) {
    Impl->ResourceBarrier(Barrier);
}

void FRHICommandContext::SetRenderTarget(const FRHITexture &Target, const FRHIRenderArea &RenderArea) {
    Impl->SetRenderTarget(Target, RenderArea);
}

void FRHICommandContext::UnsetRenderTarget() {
    Impl->UnsetRenderTarget();
}

void FRHICommandContext::BindVertexBuffer(FRHIBuffer &Buffer, FUInt32 FirstVertex) {
    Impl->BindVertexBuffer(Buffer, FirstVertex);
}

void FRHICommandContext::BindIndexBuffer(FRHIBuffer &Buffer) {
    Impl->BindIndexBuffer(Buffer);
}

void FRHICommandContext::DrawIndexed(FUInt32 IndexCount, FUInt32 InstanceCount, FUInt32 FirstIndex, FSInt32 VertexOffset, FUInt32 FirstInstance) {
    Impl->DrawIndexed(IndexCount, InstanceCount, FirstIndex, VertexOffset, FirstInstance);
}

void FRHICommandContext::BindPipeline(FRHIPipeline &Pipeline) {
    Impl->BindPipeline(Pipeline);
}

void FRHICommandContext::DrawInstanced(FUInt32 VertexCount, FUInt32 InstanceCount, FUInt32 FirstVertex, FUInt32 FirstInstance) {
    Impl->DrawInstanced(VertexCount, InstanceCount, FirstVertex, FirstInstance);
}

void FRHICommandContext::SetViewport(const FRHIViewport &Viewport) {
    Impl->SetViewport(Viewport);
}

void FRHICommandContext::SetScissor(const FRHIScissor &Scissor) {
    Impl->SetScissor(Scissor);
}