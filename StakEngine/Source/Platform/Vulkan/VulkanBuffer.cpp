#include "VulkanBuffer.h"

#include "VulkanRHI.h"

FVulkanBuffer::FVulkanBuffer(FVulkanDevice *Device, const FRHIBufferDescription &Description) : Device(Device), UseStagingBuffer(Description.UseStagingBuffer), ElementCount(Description.ElementCount), Size(Description.ElementCount * Description.Layout.Stride), Type(Description.Type), Layout(Description.Layout) {
    if (UseStagingBuffer) {
        Device->CreateVulkanBuffer(Size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &StagingBuffer, &StagingBufferMemory);

        Device->CreateVulkanBuffer(Size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &Buffer, &BufferMemory);

        vkMapMemory(Device->GetVulkanDevice(), StagingBufferMemory, 0, Size, 0, &MappedBuffer);
    } else {
        Device->CreateVulkanBuffer(Size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &Buffer, &BufferMemory);

        vkMapMemory(Device->GetVulkanDevice(), BufferMemory, 0, Size, 0, &MappedBuffer);
    }

    if (Description.InitialContents != nullptr) {
        SetData(Description.InitialContents, Description.InitialContentsSize);
    }
}

void FVulkanBuffer::SetData(void *Data, FUInt32 DataSize)  {
    memcpy(MappedBuffer, Data, DataSize);

    if (UseStagingBuffer) {
        Device->CopyVulkanBuffer(Buffer, StagingBuffer, Size);
    }
}

void *FVulkanBuffer::GetMappedBuffer() {
    return MappedBuffer;
}

void FVulkanBuffer::Shutdown() {
    if (UseStagingBuffer) {
        vkFreeMemory(Device->GetVulkanDevice(), StagingBufferMemory, nullptr);
        vkDestroyBuffer(Device->GetVulkanDevice(), StagingBuffer, nullptr);
    }

    vkFreeMemory(Device->GetVulkanDevice(), BufferMemory, nullptr);
    vkDestroyBuffer(Device->GetVulkanDevice(), Buffer, nullptr);
}
    
ERHIBufferType FVulkanBuffer::GetBufferType() {
    return Type;
}

FRHIBufferLayout FVulkanBuffer::GetLayout() {
    return Layout;
}
