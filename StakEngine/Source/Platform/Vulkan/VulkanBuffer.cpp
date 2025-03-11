#include "VulkanBuffer.h"

#include "VulkanRHI.h"

extern IRHI *GRHI;

FVulkanBuffer::FVulkanBuffer(VkDevice Device) : Device(Device) {
    Type = ERHIResourceType::BUFFER;
}

bool FVulkanBuffer::SendToGPU()  {
    if (UseStagingBuffer) {
        auto RHI = reinterpret_cast<FVulkanRHI *>(GRHI);
        if (!RHI->CopyBuffer(Buffer, StagingBuffer, Size)) {
            SK_LOG_ERROR("Failed to set buffer data");
            return false;
        }
    }

    return true;
}

bool FVulkanBuffer::Init(FRHIBufferDescription *Description) {
    UseStagingBuffer = Description->UseStagingBuffer;
    ElementCount = Description->ElementCount;
    Size = Description->ElementCount * Description->Layout.Stride;
    BufferType = Description->Type;
    Layout = Description->Layout;

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

        if (vkMapMemory(Device, StagingBufferMemory, 0, Size, 0, &MappedData) != VK_SUCCESS) {
            SK_LOG_ERROR("Failed to map buffer memory");
            return false;
        }
    } else {
        if (!RHI->CreateBuffer(Size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &Buffer, &BufferMemory)) {
            SK_LOG_ERROR("Failed to create device buffer");
            return false;
        }

        if (vkMapMemory(Device, BufferMemory, 0, Size, 0, &MappedData) != VK_SUCCESS) {
            SK_LOG_ERROR("Failed to map buffer memory");
            return false;
        }
    }

    if (Description->InitialContents != nullptr) {
        if (Description->InitialContentsSize > Size) {
            SK_LOG_WARN("Initial contents size is greater thatn buffer size");
            return true;
        }

        memcpy(MappedData, Description->InitialContents, Description->InitialContentsSize);
        if (!SendToGPU()) {
            SK_LOG_ERROR("Failed to send buffer to gpu");
            return false;
        }
    }

    return true;
}

void FVulkanBuffer::Shutdown() {
    if (UseStagingBuffer) {
        vkFreeMemory(Device, StagingBufferMemory, nullptr);
        vkDestroyBuffer(Device, StagingBuffer, nullptr);
    }

    vkFreeMemory(Device, BufferMemory, nullptr);
    vkDestroyBuffer(Device, Buffer, nullptr);
}