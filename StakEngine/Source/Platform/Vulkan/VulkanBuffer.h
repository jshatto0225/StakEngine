#include "RHIBuffer.h"

#include "VulkanRHI.h"

class FVulkanBuffer : public IRHIBuffer {
public:
    FVulkanBuffer(VkDevice Device);

    bool SetData(void *Data, FUInt32 DataSize) override;
    void *GetMappedBuffer() override;

    FRHIBufferLayout GetLayout() override;

    FUInt32 GetElementCount() override { return ElementCount; }

    bool Init(FRHIBufferDescription *Description) override;
    void Shutdown() override;

    ERHIBufferType GetBufferType() override;

public:
    inline VkBuffer GetVulkanBuffer() { return Buffer; }

private:
    VkDevice Device;

    bool UseStagingBuffer = false;

    VkBuffer Buffer = VK_NULL_HANDLE;
    VkBuffer StagingBuffer = VK_NULL_HANDLE;

    VkDeviceMemory BufferMemory = VK_NULL_HANDLE;
    VkDeviceMemory StagingBufferMemory = VK_NULL_HANDLE;

    FUInt32 ElementCount = 0;
    FUInt32 Size = 0;

    ERHIBufferType Type = ERHIBufferType::VERTEX;
    FRHIBufferLayout Layout;

    void *MappedBuffer = nullptr;
};
