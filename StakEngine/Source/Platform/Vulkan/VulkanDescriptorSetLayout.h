#pragma once

#include "RHIDescriptorSetLayout.h"
#include "VulkanDevice.h"

class FVulkanDescriptorSetLayout : public IRHIDescriptorSetLayout {
public:
    FVulkanDescriptorSetLayout(FVulkanDevice &Device, const FRHIDescriptorSetLayoutDescription &Description);
    
    void Shutdown() override;
    
public:
    inline VkDescriptorSetLayout GetLayout() const { return Layout; }
    
private:
    FVulkanDevice &Device;
    
    VkDescriptorSetLayout Layout = VK_NULL_HANDLE;
};