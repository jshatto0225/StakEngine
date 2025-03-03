#pragma once

#include "RHICommandContext.h"

#include "VulkanRHI.h"
#include "Log.h"
#include "Types.h"

#include "VulkanDevice.h"

#include <vector>

extern FRHI *GRHI;

class FVulkanCommandContext : public IRHICommandContext {
public:
    FVulkanCommandContext(FVulkanDevice &Device);

    void Begin() override;
    void End() override;

    void ResourceBarrier(const FRHIResourceBarrier& Barrier) override;

    void SetRenderTarget(const FRHITexture &Target, const FRHIRenderArea &RenderArea) override;
    void UnsetRenderTarget() override;

    void BindVertexBuffer(FRHIBuffer &Buffer, FUInt32 FirstVertex) override;
    void BindIndexBuffer(FRHIBuffer &Buffer) override;

    void DrawIndexed(FUInt32 IndexCount, FUInt32 InstanceCount, FUInt32 FirstIndex, FSInt32 VertexOffset, FUInt32 FirstInstance) override;

    void RenderImGuiDrawData(ImDrawData *DrawData) override;

    void Shutdown() override;

public:
    void TransitionBarrier(const FRHITransitionBarrier &Barrier);

public:
    VkCommandBuffer GetMainCommandBuffer();

private:
    bool Initialized = true;

    FVulkanDevice &Device;

    bool Active = false;
    VkCommandPool CommandPool = VK_NULL_HANDLE;
    std::vector<VkCommandBuffer> MainCommandBuffers = {};
};
