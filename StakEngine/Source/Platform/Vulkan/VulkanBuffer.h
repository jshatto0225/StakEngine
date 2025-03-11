#include "RHIBuffer.h"

#include "VulkanRHI.h"

struct FVulkanBuffer : public IRHIBuffer {
    FVulkanBuffer(VkDevice Device);

    bool Init(FRHIBufferDescription *Description) override;
    void Shutdown() override;

    bool SendToGPU() override;

    VkDevice Device;
    bool UseStagingBuffer = false;
    VkBuffer Buffer = VK_NULL_HANDLE;
    VkBuffer StagingBuffer = VK_NULL_HANDLE;
    VkDeviceMemory BufferMemory = VK_NULL_HANDLE;
    VkDeviceMemory StagingBufferMemory = VK_NULL_HANDLE;
};
