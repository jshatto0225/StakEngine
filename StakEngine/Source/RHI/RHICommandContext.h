#pragma once

#include "RHIForward.h"
#include "RHIResource.h"
#include "RHIDevice.h"

#include <imgui.h>

class IRHICommandContext {
public:
    virtual ~IRHICommandContext() = default;

    virtual void Begin() = 0;
    virtual void End() = 0;

    virtual void RenderImGuiDrawData(ImDrawData *DrawData) = 0;

    virtual void ResourceBarrier(const FRHIResourceBarrier &Barrier) = 0;

    // TODO: Allow multiple render targets
    virtual void SetRenderTarget(const FRHITexture &Target, const FRHIRenderArea &RenderArea) = 0;
    virtual void UnsetRenderTarget() = 0;

    // TODO: Bind multiple buffer at once
    virtual void BindVertexBuffer(FRHIBuffer &Buffer, FUInt32 FirstVertex) = 0;
    virtual void BindIndexBuffer(FRHIBuffer &Buffer) = 0;
    
    virtual void DrawIndexed(FUInt32 IndexCount, FUInt32 InstanceCount, FUInt32 FirstIndex, FSInt32 VertexOffset, FUInt32 FirstInstance) = 0;
    virtual void DrawInstanced(FUInt32 VertexCount, FUInt32 InstanceCount, FUInt32 FirstVertex, FUInt32 FirstInstance) = 0;

	virtual void SetViewport(const FRHIViewport &Viewport) = 0;
	virtual void SetScissor(const FRHIScissor &Scissor) = 0;

    virtual void BindPipeline(FRHIPipeline &Pipeline) = 0;

    virtual void Shutdown() = 0;
};

class FRHICommandContext {
public:
    FRHICommandContext() = default;

public:
    FRHICommandContext(TRef<IRHICommandContext> Impl);

    void Begin();
    void End();

    void RenderImGuiDrawData(ImDrawData* DrawData);

    void ResourceBarrier(const FRHIResourceBarrier &Barrier);

    // TODO: Allow multiple render targets
    void SetRenderTarget(const FRHITexture &Target, const FRHIRenderArea &RenderArea);
    void UnsetRenderTarget();

    // TODO: Bind multiple vertex buffers at once
    void BindVertexBuffer(FRHIBuffer &Buffer, FUInt32 FirstVertex);
    void BindIndexBuffer(FRHIBuffer &Buffer);

	void DrawIndexed(FUInt32 IndexCount, FUInt32 InstanceCount, FUInt32 FirstIndex, FSInt32 VertexOffset, FUInt32 FirstInstance);
	void DrawInstanced(FUInt32 VertexCount, FUInt32 InstanceCount, FUInt32 FirstVertex, FUInt32 FirstInstance);

	void SetViewport(const FRHIViewport &Viewport);
	void SetScissor(const FRHIScissor &Scissor);

	void BindPipeline(FRHIPipeline &Pipeline);

    void Shutdown();

    inline TRef<IRHICommandContext> GetImpl() { return Impl; }

private:
    TRef<IRHICommandContext> Impl = nullptr;
};
