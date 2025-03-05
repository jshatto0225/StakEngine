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
    virtual void SetRenderTarget(TRef<IRHITexture> Target, const FRHIRenderArea &RenderArea) = 0;
    virtual void UnsetRenderTarget() = 0;

    // TODO: Bind multiple buffer at once
    virtual void BindVertexBuffer(TRef<IRHIBuffer> Buffer, FUInt32 FirstVertex) = 0;
    virtual void BindIndexBuffer(TRef<IRHIBuffer> Buffer) = 0;
    
    virtual void DrawIndexed(FUInt32 IndexCount, FUInt32 InstanceCount, FUInt32 FirstIndex, FSInt32 VertexOffset, FUInt32 FirstInstance) = 0;
    virtual void DrawInstanced(FUInt32 VertexCount, FUInt32 InstanceCount, FUInt32 FirstVertex, FUInt32 FirstInstance) = 0;

    virtual void SetViewport(FFloat X, FFloat Y, FFloat Width, FFloat Height, FFloat MinDepth, FFloat MaxDepth) = 0;
    virtual void SetScissor(FSInt32 X, FSInt32 Y, FUInt32 Width, FUInt32 Height) = 0;

    virtual void BindPipeline(TRef<IRHIPipeline> Pipeline) = 0;

    virtual void Shutdown() = 0;
};
