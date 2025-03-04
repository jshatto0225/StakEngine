#pragma once

#include "RHIPipeline.h"
#include "VulkanDevice.h"

class FVulkanPipelineLayout : public IRHIPipelineLayout {
public:
    FVulkanPipelineLayout(FVulkanDevice &Device, const FRHIPipelineLayoutDescription &Description);
    
    void Shutdown() override;
    
public:
    VkPipelineLayout GetLayout() { return Layout; }
    
private:
    FVulkanDevice &Device;
    
    VkPipelineLayout Layout = VK_NULL_HANDLE;
};

class FVulkanPipeline : public IRHIPipeline {
public:
    FVulkanPipeline(FVulkanDevice &Device, const FRHIGraphicsPipelineStateDescription &Description);
    
    void Shutdown() override;
    
public:
    inline VkPipeline GetVulkanPipeline() { return Pipeline; }
    inline VkPipelineBindPoint GetVulkanBindPoint() { return BindPoint; }
    
private:
    FVulkanDevice &Device;

    VkPipelineBindPoint BindPoint;
    
    VkPipeline Pipeline = VK_NULL_HANDLE;    
};