#include "VulkanBuffer.h"

#include "VulkanRHI.h"

extern IRHI *GRHI;

FVulkanBuffer::FVulkanBuffer(VkDevice Device) : Device(Device) {}

bool FVulkanBuffer::SetData(void *Data, FUInt32 DataSize)  {
    memcpy(MappedBuffer, Data, DataSize);
    if (UseStagingBuffer) {
        auto RHI = reinterpret_cast<FVulkanRHI *>(GRHI);
        if (!RHI->CopyBuffer(Buffer, StagingBuffer, Size)) {
            SK_LOG_ERROR("Failed to set buffer data");
            return false;
        }
    }

    return true;
}

bool FVulkanBuffer::Init(const FRHIBufferDescription &Description) {
    UseStagingBuffer = Description.UseStagingBuffer;
    ElementCount = Description.ElementCount;
    Size = Description.ElementCount * Description.Layout.Stride;
    Type = Description.Type;
    Layout = Description.Layout;

    auto RHI = reinterpret_cast<FVulkanRHI *>(GRHI);
    if (UseStagingBuffer) {
        if (!RHI->CreateBuffer(Size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &StagingBuffer, &StagingBufferMemory)) {
            SK_LOG_ERROR("Failed to create staging buffer");
            return false;
        }

        if (!RHI->CreateBuffer(Size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &Buffer, &BufferMemory)) {
            SK_LOG_ERROR("Failed to create device buffer");
            return false;
        }

        if (vkMapMemory(Device, StagingBufferMemory, 0, Size, 0, &MappedBuffer) != VK_SUCCESS) {
            SK_LOG_ERROR("Failed to map buffer memory");
            return false;
        }
    } else {
        if (!RHI->CreateBuffer(Size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &Buffer, &BufferMemory)) {
            SK_LOG_ERROR("Failed to create device buffer");
            return false;
        }

        if (vkMapMemory(Device, BufferMemory, 0, Size, 0, &MappedBuffer) != VK_SUCCESS) {
            SK_LOG_ERROR("Failed to map buffer memory");
            return false;
        }
    }

    if (Description.InitialContents != nullptr) {
        if (!SetData(Description.InitialContents, Description.InitialContentsSize)) {
            SK_LOG_ERROR("Failed to set initial buffer contents");
            return false;
        }
    }

    return true;
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
