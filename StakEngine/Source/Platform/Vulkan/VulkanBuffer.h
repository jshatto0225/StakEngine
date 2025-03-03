#include "RHIBuffer.h"

#include "VulkanDevice.h"

class FVulkanBuffer : public IRHIBuffer {
public:
    FVulkanBuffer(FVulkanDevice &Device, const FRHIBufferDescription &Description);

    void SetData(void *Data, FUInt32 DataSize) override;
    void *GetMappedBuffer() override;

    FRHIBufferLayout GetLayout() override;
    
    FUInt32 GetElementCount() override { return ElementCount; }
    
    void Shutdown() override;
    
    ERHIBufferType GetBufferType() override;

public:
    inline VkBuffer GetVulkanBuffer() { return Buffer; }
    
private:
    FVulkanDevice &Device;
    
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
