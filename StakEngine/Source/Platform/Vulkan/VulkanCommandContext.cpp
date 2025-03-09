#include "VulkanCommandContext.h"

#include "VulkanTexture.h"
#include "VulkanBuffer.h"
#include "VulkanRHI.h"
#include "VulkanPipeline.h"

#include <backends/imgui_impl_vulkan.h>

extern IRHI *GRHI;

FVulkanCommandContext::FVulkanCommandContext(VkDevice Device) : Device(Device) {}

void FVulkanCommandContext::Shutdown() {
    vkFreeCommandBuffers(Device, CommandPool, static_cast<FUInt32>(MainCommandBuffers.size()), MainCommandBuffers.data());
    vkDestroyCommandPool(Device, CommandPool, nullptr);
}

bool FVulkanCommandContext::Init() {
    VkCommandPoolCreateInfo CommandPoolInfo = {};

    CommandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    CommandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    auto RHI = reinterpret_cast<FVulkanRHI *>(GRHI);
    CommandPoolInfo.queueFamilyIndex = RHI->GetGraphicsQueueIndex();

    if (vkCreateCommandPool(Device, &CommandPoolInfo, nullptr, &CommandPool) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to create vulkan command pool");
        return false;
    }

    VkCommandBufferAllocateInfo AllocInfo = {};
    AllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    AllocInfo.commandPool = CommandPool;
    AllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    AllocInfo.commandBufferCount = MAX_FRAMES_IN_FLIGHT;

    MainCommandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    if (vkAllocateCommandBuffers(Device, &AllocInfo, MainCommandBuffers.data()) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to allocate vulkan command buffers");
        return false;
    }

    return true;
}

bool FVulkanCommandContext::Begin() {
    auto RHI = reinterpret_cast<FVulkanRHI *>(GRHI);
    if (vkResetCommandBuffer(MainCommandBuffers[RHI->GetCurrentFrameIndex()], 0) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to reset command buffer");
        return false;
    }

    VkCommandBufferBeginInfo BeginInfo = {};
    BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    if (vkBeginCommandBuffer(MainCommandBuffers[RHI->GetCurrentFrameIndex()], &BeginInfo) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to begin command buffer");
        return false;
    }

    return true;
}

bool FVulkanCommandContext::End() {
    auto RHI = reinterpret_cast<FVulkanRHI *>(GRHI);
    if (vkEndCommandBuffer(MainCommandBuffers[RHI->GetCurrentFrameIndex()]) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to end command buffer");
        return false;
    }

    return true;
}

void FVulkanCommandContext::RenderImGuiDrawData(ImDrawData* DrawData) {
    auto RHI = reinterpret_cast<FVulkanRHI *>(GRHI);
    ImGui_ImplVulkan_RenderDrawData(DrawData, MainCommandBuffers[RHI->GetCurrentFrameIndex()]);
}

VkCommandBuffer FVulkanCommandContext::GetMainCommandBuffer() {
    auto RHI = reinterpret_cast<FVulkanRHI *>(GRHI);
    return MainCommandBuffers[RHI->GetCurrentFrameIndex()];
}

void FVulkanCommandContext::ResourceBarrier(const FRHIResourceBarrier &Barrier) {
    switch (Barrier.Type) {
    case ERHIBarrierType::TRANSITION:
        TransitionBarrier(Barrier.TransitionBarrier);
        break;
    default:
        SK_LOG_WARN("Unsupported resource barrier type");
        return;
    }
}

void FVulkanCommandContext::TransitionBarrier(const FRHITransitionBarrier& Barrier) {
    switch (RHIGetTransitionType(Barrier.StateBefore, Barrier.StateAfter)) {
    case ERHITransitionType::IMAGE: {
        if (Barrier.Resource->GetType() != ERHIResourceType::TEXTURE) {
            SK_LOG_ERROR("Resource for image transition was not an image");
            return;
        }

        auto RHI = reinterpret_cast<FVulkanRHI *>(GRHI);

        auto Texture = std::static_pointer_cast<FVulkanTexture>(Barrier.Resource);

        VkImageMemoryBarrier ImageBarrier = {};
        ImageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        ImageBarrier.srcAccessMask = GetVulkanAccessMask(Barrier.StateBefore);
        ImageBarrier.dstAccessMask = GetVulkanAccessMask(Barrier.StateAfter);
        ImageBarrier.oldLayout = GetVulkanImageLayout(Barrier.StateBefore);
        ImageBarrier.newLayout = GetVulkanImageLayout(Barrier.StateAfter);
        EVulkanQueue SrcQueue = GetVulkanQueue(Barrier.StateAfter);
        if (SrcQueue == EVulkanQueue::ANY) {
            ImageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        } else if (SrcQueue == EVulkanQueue::GRAPHICS) {
            ImageBarrier.srcQueueFamilyIndex = RHI->GetGraphicsQueueIndex();
        } else if (SrcQueue == EVulkanQueue::PRESENT) {
            ImageBarrier.srcQueueFamilyIndex = RHI->GetActivePresentQueueIndex();
        } else {
            SK_LOG_ERROR("Unsupported source queue");
            return;
        }
        EVulkanQueue DstQueue = GetVulkanQueue(Barrier.StateAfter);
        if (DstQueue == EVulkanQueue::ANY) {
            ImageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        } else if (DstQueue == EVulkanQueue::GRAPHICS) {
            ImageBarrier.dstQueueFamilyIndex = RHI->GetGraphicsQueueIndex();
        } else if (DstQueue == EVulkanQueue::PRESENT) {
            ImageBarrier.dstQueueFamilyIndex = RHI->GetActivePresentQueueIndex();
        } else {
            SK_LOG_ERROR("Unsupported destination queue");
            return;
        }
        if (Texture->IsBackbuffer()) {
            ImageBarrier.image = Texture->GetVulkanImage(RHI->GetCurrentImageIndex());
        } else {
            ImageBarrier.image = Texture->GetVulkanImage(RHI->GetCurrentFrameIndex());
        }
        ImageBarrier.subresourceRange = Texture->GetVulkanSubresourceRange(Barrier.Subresource);

        // TODO: Batch pipeline barrier calls
        vkCmdPipelineBarrier(MainCommandBuffers[RHI->GetCurrentFrameIndex()], GetVulkanPipelineStageMask(Barrier.StateBefore), GetVulkanPipelineStageMask(Barrier.StateAfter), 0 /* TODO: Dependency Flags */, 0, nullptr, 0, nullptr, 1, &ImageBarrier);
        break;
    }
    default:
        SK_LOG_WARN("Unsupported resource transition type");
        return;
    }
}

void FVulkanCommandContext::SetRenderTarget(const TRef<IRHITexture> Target, const FRHIRenderArea &RenderArea) {
    auto VulkanTarget = std::static_pointer_cast<FVulkanTexture>(Target);

    auto RHI = reinterpret_cast<FVulkanRHI *>(GRHI);

    VkRenderingAttachmentInfo AttachmentInfo = {};
    AttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    AttachmentInfo.clearValue.color.float32[0] = 1.0f;
    AttachmentInfo.clearValue.color.float32[1] = 0.0f;
    AttachmentInfo.clearValue.color.float32[2] = 1.0f;
    AttachmentInfo.clearValue.color.float32[3] = 1.0f;
    if (VulkanTarget->IsBackbuffer()) {
        AttachmentInfo.imageView = VulkanTarget->GetVulkanImageView(RHI->GetCurrentImageIndex());
    } else {
        AttachmentInfo.imageView = VulkanTarget->GetVulkanImageView(RHI->GetCurrentFrameIndex());
    }
    AttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    AttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    AttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    VkRenderingInfo RenderingInfo = {};
    RenderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    RenderingInfo.colorAttachmentCount = 1;
    RenderingInfo.pColorAttachments = &AttachmentInfo;
    RenderingInfo.viewMask = 0x01;
    RenderingInfo.layerCount = RenderArea.LayerCount;
    RenderingInfo.renderArea.extent = { RenderArea.X, RenderArea.Y };
    RenderingInfo.renderArea.extent = { RenderArea.Width, RenderArea.Height };
    vkCmdBeginRendering(MainCommandBuffers[RHI->GetCurrentFrameIndex()], &RenderingInfo);
}

void FVulkanCommandContext::UnsetRenderTarget() {
    auto RHI = reinterpret_cast<FVulkanRHI *>(GRHI);
    vkCmdEndRendering(MainCommandBuffers[RHI->GetCurrentFrameIndex()]);
}

void FVulkanCommandContext::BindVertexBuffer(TRef<IRHIBuffer> Buffer, FUInt32 FirstVertex) {
    auto Impl = std::static_pointer_cast<FVulkanBuffer>(Buffer);
    auto RHI = reinterpret_cast<FVulkanRHI *>(GRHI);
    VkDeviceSize Offset = 0;
    VkBuffer Buffers[] = { Impl->GetVulkanBuffer() };
    vkCmdBindVertexBuffers(MainCommandBuffers[RHI->GetCurrentFrameIndex()], FirstVertex, 1, Buffers, &Offset);
}

void FVulkanCommandContext::BindIndexBuffer(TRef<IRHIBuffer> Buffer) {
    auto Impl = std::static_pointer_cast<FVulkanBuffer>(Buffer);
    auto RHI = reinterpret_cast<FVulkanRHI *>(GRHI);
    vkCmdBindIndexBuffer(MainCommandBuffers[RHI->GetCurrentFrameIndex()], Impl->GetVulkanBuffer(), 0, GetVulkanIndexType(Buffer->GetLayout().Elements[0].Format));
}

void FVulkanCommandContext::DrawIndexed(FUInt32 IndexCount, FUInt32 InstanceCount, FUInt32 FirstIndex, FSInt32 VertexOffset, FUInt32 FirstInstance) {
    auto RHI = reinterpret_cast<FVulkanRHI *>(GRHI);
    vkCmdDrawIndexed(MainCommandBuffers[RHI->GetCurrentFrameIndex()], IndexCount, InstanceCount, FirstIndex, VertexOffset, FirstInstance);
}

void FVulkanCommandContext::BindPipeline(TRef<IRHIPipeline> Pipeline) {
    auto RHI = reinterpret_cast<FVulkanRHI *>(GRHI);
    auto Impl = std::static_pointer_cast<FVulkanPipeline>(Pipeline);
    vkCmdBindPipeline(MainCommandBuffers[RHI->GetCurrentFrameIndex()], Impl->GetVulkanBindPoint(), Impl->GetVulkanPipeline());
}

void FVulkanCommandContext::DrawInstanced(FUInt32 VertexCount, FUInt32 InstanceCount, FUInt32 FirstVertex, FUInt32 FirstInstance) {
    auto RHI = reinterpret_cast<FVulkanRHI *>(GRHI);
    vkCmdDraw(MainCommandBuffers[RHI->GetCurrentFrameIndex()], VertexCount, InstanceCount, FirstVertex, FirstInstance);
}

void FVulkanCommandContext::SetViewport(FFloat X, FFloat Y, FFloat Width, FFloat Height, FFloat MinDepth, FFloat MaxDepth) {
    auto RHI = reinterpret_cast<FVulkanRHI *>(GRHI);
    VkViewport VulkanViewport = {
        X,
        Y,
        Width,
        Height,
        MinDepth,
        MaxDepth
    };

    vkCmdSetViewport(MainCommandBuffers[RHI->GetCurrentFrameIndex()], 0, 1, &VulkanViewport);
}

void FVulkanCommandContext::SetScissor(FSInt32 X, FSInt32 Y, FUInt32 Width, FUInt32 Height) {
    auto RHI = reinterpret_cast<FVulkanRHI *>(GRHI);
    VkRect2D VulkanScissor = {
        { X,     Y      },
        { Width, Height }
    };

    vkCmdSetScissor(MainCommandBuffers[RHI->GetCurrentFrameIndex()], 0, 1, &VulkanScissor);
}
