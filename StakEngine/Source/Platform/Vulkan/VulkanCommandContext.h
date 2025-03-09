#pragma once

#include "RHICommandContext.h"

#include "VulkanRHI.h"
#include "Log.h"
#include "Types.h"

#include <vector>

class FVulkanCommandContext : public IRHICommandContext {
public:
    FVulkanCommandContext(VkDevice Device);

    bool Begin() override;
    bool End() override;

    void ResourceBarrier(const FRHIResourceBarrier& Barrier) override;

    void SetRenderTarget(const TRef<IRHITexture> Target, const FRHIRenderArea &RenderArea) override;
    void UnsetRenderTarget() override;

    void BindVertexBuffer(TRef<IRHIBuffer> Buffer, FUInt32 FirstVertex) override;
    void BindIndexBuffer(TRef<IRHIBuffer> Buffer) override;

    void DrawIndexed(FUInt32 IndexCount, FUInt32 InstanceCount, FUInt32 FirstIndex, FSInt32 VertexOffset, FUInt32 FirstInstance) override;
    void DrawInstanced(FUInt32 VertexCount, FUInt32 InstanceCount, FUInt32 FirstVertex, FUInt32 FirstInstance) override;

    void SetViewport(FFloat X, FFloat Y, FFloat Width, FFloat Height, FFloat MinDepth, FFloat MaxDepth) override;
    void SetScissor(FSInt32 X, FSInt32 Y, FUInt32 Width, FUInt32 Height) override;

    void BindPipeline(TRef<IRHIPipeline> Pipeline) override;

    void RenderImGuiDrawData(ImDrawData *DrawData) override;

    bool Init() override;
    void Shutdown() override;

public:
    void TransitionBarrier(const FRHITransitionBarrier &Barrier);

public:
    VkCommandBuffer GetMainCommandBuffer();

private:
    VkDevice Device;

    VkCommandPool CommandPool = VK_NULL_HANDLE;
    std::vector<VkCommandBuffer> MainCommandBuffers = {};
};
