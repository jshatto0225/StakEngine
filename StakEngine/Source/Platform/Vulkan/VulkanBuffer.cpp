#include "VulkanBuffer.h"

#include "VulkanRHI.h"

extern IRHI *GRHI;

FVulkanBuffer::FVulkanBuffer(VkDevice Device, const FRHIBufferDescription &Description) : Device(Device), UseStagingBuffer(Description.UseStagingBuffer), ElementCount(Description.ElementCount), Size(Description.ElementCount * Description.Layout.Stride), Type(Description.Type), Layout(Description.Layout) {
    auto RHI = reinterpret_cast<FVulkanRHI *>(GRHI);
    if (UseStagingBuffer) {
        RHI->CreateBuffer(Size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &StagingBuffer, &StagingBufferMemory);

        RHI->CreateBuffer(Size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &Buffer, &BufferMemory);

        vkMapMemory(Device, StagingBufferMemory, 0, Size, 0, &MappedBuffer);
    } else {
        RHI->CreateBuffer(Size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &Buffer, &BufferMemory);

        vkMapMemory(Device, BufferMemory, 0, Size, 0, &MappedBuffer);
    }

    if (Description.InitialContents != nullptr) {
        SetData(Description.InitialContents, Description.InitialContentsSize);
    }
}

void FVulkanBuffer::SetData(void *Data, FUInt32 DataSize)  {
    memcpy(MappedBuffer, Data, DataSize);
    if (UseStagingBuffer) {
        auto RHI = reinterpret_cast<FVulkanRHI *>(GRHI);
        RHI->CopyBuffer(Buffer, StagingBuffer, Size);
    }
}

void *FVulkanBuffer::GetMappedBuffer() {
    return MappedBuffer;
}

void FVulkanBuffer::Shutdown() {
    if (UseStagingBuffer) {
        vkFreeMemory(Device, StagingBufferMemory, nullptr);
        vkDestroyBuffer(Device, StagingBuffer, nullptr);
    }

    vkFreeMemory(Device, BufferMemory, nullptr);
    vkDestroyBuffer(Device, Buffer, nullptr);
}
    
ERHIBufferType FVulkanBuffer::GetBufferType() {
    return Type;
}

FRHIBufferLayout FVulkanBuffer::GetLayout() {
    return Layout;
}
