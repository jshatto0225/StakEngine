#include "VulkanCommandContext.h"

#include "VulkanTexture.h"
#include "VulkanBuffer.h"
#include "VulkanRHI.h"
#include "VulkanPipeline.h"

#include <backends/imgui_impl_vulkan.h>

FVulkanCommandContext::FVulkanCommandContext(FVulkanDevice &Device) : Device(Device) {
    VkCommandPoolCreateInfo CommandPoolInfo = {};
    
    CommandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    CommandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    CommandPoolInfo.queueFamilyIndex = Device.GetGraphicsQueueIndex();
    
    CHECK_VK_ERR(vkCreateCommandPool(Device.GetVulkanDevice(), &CommandPoolInfo, nullptr, &CommandPool), "Failed to create vulkan command pool");
    
    VkCommandBufferAllocateInfo AllocInfo = {};
    AllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    AllocInfo.commandPool = CommandPool;
    AllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    AllocInfo.commandBufferCount = MAX_FRAMES_IN_FLIGHT;
    
    MainCommandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    CHECK_VK_ERR(vkAllocateCommandBuffers(Device.GetVulkanDevice(), &AllocInfo, MainCommandBuffers.data()), "Failed to allocate vulkan command buffers");
}

void FVulkanCommandContext::Shutdown() {
    vkFreeCommandBuffers(Device.GetVulkanDevice(), CommandPool, static_cast<FUInt32>(MainCommandBuffers.size()), MainCommandBuffers.data());
    vkDestroyCommandPool(Device.GetVulkanDevice(), CommandPool, nullptr);

    Initialized = false;
}

void FVulkanCommandContext::Begin() {
    if (Active) {
        SK_LOG_WARN("Command context has already begun");
        return;
    }

    Active = true;

    vkResetCommandBuffer(MainCommandBuffers[Device.GetCurrentFrameIndex()], 0);

    VkCommandBufferBeginInfo BeginInfo = {};
    BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    CHECK_VK_ERR(vkBeginCommandBuffer(MainCommandBuffers[Device.GetCurrentFrameIndex()], &BeginInfo), "Failed to begin command buffer");
}

void FVulkanCommandContext::End() {
    if (!Active) {
        SK_LOG_WARN("Command context not started");
        return;
    }

    vkEndCommandBuffer(MainCommandBuffers[Device.GetCurrentFrameIndex()]);

    Active = false;
}

void FVulkanCommandContext::RenderImGuiDrawData(ImDrawData* DrawData) {
    if (!Active) {
        SK_LOG_WARN("Command context is not active");
        return;
    }

    ImGui_ImplVulkan_RenderDrawData(DrawData, MainCommandBuffers[Device.GetCurrentFrameIndex()]);
}

VkCommandBuffer FVulkanCommandContext::GetMainCommandBuffer() {
    return MainCommandBuffers[Device.GetCurrentFrameIndex()];
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
        if (Barrier.Resource.GetType() != ERHIResourceType::TEXTURE) {
            SK_LOG_ERROR("Resource for image transition was not an image");
            return;
        }

        auto Texture = std::static_pointer_cast<FVulkanTexture>(Barrier.Resource.GetImpl());

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
            ImageBarrier.srcQueueFamilyIndex = Device.GetGraphicsQueueIndex();
        } else if (SrcQueue == EVulkanQueue::PRESENT) {
            ImageBarrier.srcQueueFamilyIndex = Device.GetPresentQueueIndex();
        } else {
            SK_LOG_ERROR("Unsupported source queue");
            return;
        }
        EVulkanQueue DstQueue = GetVulkanQueue(Barrier.StateAfter);
        if (DstQueue == EVulkanQueue::ANY) {
            ImageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        } else if (DstQueue == EVulkanQueue::GRAPHICS) {
            ImageBarrier.dstQueueFamilyIndex = Device.GetGraphicsQueueIndex();
        } else if (DstQueue == EVulkanQueue::PRESENT) {
            ImageBarrier.dstQueueFamilyIndex = Device.GetPresentQueueIndex();
        } else {
            SK_LOG_ERROR("Unsupported destination queue");
            return;
        }
        if (Texture->IsBackbuffer()) {
            ImageBarrier.image = Texture->GetVulkanImage(Device.GetCurrentImageIndex());
        } else {
            ImageBarrier.image = Texture->GetVulkanImage(Device.GetCurrentFrameIndex());
        }
        ImageBarrier.subresourceRange = Texture->GetVulkanSubresourceRange(Barrier.Subresource);

        // TODO: Batch pipeline barrier calls
        vkCmdPipelineBarrier(MainCommandBuffers[Device.GetCurrentFrameIndex()], GetVulkanPipelineStageMask(Barrier.StateBefore), GetVulkanPipelineStageMask(Barrier.StateAfter), 0 /* TODO: Dependency Flags */, 0, nullptr, 0, nullptr, 1, &ImageBarrier);
        break;
    }
    default:
        SK_LOG_WARN("Unsupported resource transition type");
        return;
    }
}

void FVulkanCommandContext::SetRenderTarget(const FRHITexture &Target, const FRHIRenderArea &RenderArea) {
    auto VulkanTarget = std::static_pointer_cast<FVulkanTexture>(Target.GetImpl());

    VkRenderingAttachmentInfo AttachmentInfo = {};
    AttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    AttachmentInfo.clearValue.color.float32[0] = 1.0f;
    AttachmentInfo.clearValue.color.float32[1] = 0.0f;
    AttachmentInfo.clearValue.color.float32[2] = 1.0f;
    AttachmentInfo.clearValue.color.float32[3] = 1.0f;
    if (VulkanTarget->IsBackbuffer()) {
        AttachmentInfo.imageView = VulkanTarget->GetVulkanImageView(Device.GetCurrentImageIndex());
    } else {
        AttachmentInfo.imageView = VulkanTarget->GetVulkanImageView(Device.GetCurrentFrameIndex());
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
    vkCmdBeginRendering(MainCommandBuffers[Device.GetCurrentFrameIndex()], &RenderingInfo);
}

void FVulkanCommandContext::UnsetRenderTarget() {
    vkCmdEndRendering(MainCommandBuffers[Device.GetCurrentFrameIndex()]);
}

void FVulkanCommandContext::BindVertexBuffer(FRHIBuffer &Buffer, FUInt32 FirstVertex) {
    auto Impl = std::static_pointer_cast<FVulkanBuffer>(Buffer.GetImpl());
    VkDeviceSize Offset = 0;
    VkBuffer Buffers[] = { Impl->GetVulkanBuffer() };
    vkCmdBindVertexBuffers(MainCommandBuffers[Device.GetCurrentFrameIndex()], FirstVertex, 1, Buffers, &Offset);
}

void FVulkanCommandContext::BindIndexBuffer(FRHIBuffer &Buffer) {
    auto Impl = std::static_pointer_cast<FVulkanBuffer>(Buffer.GetImpl());
    vkCmdBindIndexBuffer(MainCommandBuffers[Device.GetCurrentFrameIndex()], Impl->GetVulkanBuffer(), 0, GetVulkanIndexType(Buffer.GetLayout().Elements[0].Format));
}

void FVulkanCommandContext::DrawIndexed(FUInt32 IndexCount, FUInt32 InstanceCount, FUInt32 FirstIndex, FSInt32 VertexOffset, FUInt32 FirstInstance) {
    vkCmdDrawIndexed(MainCommandBuffers[Device.GetCurrentFrameIndex()], IndexCount, InstanceCount, FirstIndex, VertexOffset, FirstInstance);
}

void FVulkanCommandContext::BindPipeline(FRHIPipeline &Pipeline) {
    auto Impl = std::static_pointer_cast<FVulkanPipeline>(Pipeline.GetImpl());
    vkCmdBindPipeline(MainCommandBuffers[Device.GetCurrentFrameIndex()], Impl->GetVulkanBindPoint(), Impl->GetVulkanPipeline());
}

void FVulkanCommandContext::DrawInstanced(FUInt32 VertexCount, FUInt32 InstanceCount, FUInt32 FirstVertex, FUInt32 FirstInstance) {
    vkCmdDraw(MainCommandBuffers[Device.GetCurrentFrameIndex()], VertexCount, InstanceCount, FirstVertex, FirstInstance);
}

void FVulkanCommandContext::SetViewport(const FRHIViewport &Viewport) {
    VkViewport VulkanViewport = {};
	VulkanViewport.x = Viewport.X;
	VulkanViewport.y = Viewport.Y;
	VulkanViewport.width = Viewport.Width;
	VulkanViewport.height = Viewport.Height;
	VulkanViewport.minDepth = Viewport.MinDepth;
	VulkanViewport.maxDepth = Viewport.MaxDepth;
    vkCmdSetViewport(MainCommandBuffers[Device.GetCurrentFrameIndex()], 0, 1, &VulkanViewport);
}

void FVulkanCommandContext::SetScissor(const FRHIScissor &Scissor) {
    VkRect2D VulkanScissor = {};
    VulkanScissor.offset = { Scissor.X, Scissor.Y };
    VulkanScissor.extent = { Scissor.Width, Scissor.Height };

    vkCmdSetScissor(MainCommandBuffers[Device.GetCurrentFrameIndex()], 0, 1, &VulkanScissor);
}