#pragma once

#include "RHIShader.h"

#include "VulkanDevice.h"

class FVulkanShader : public IRHIShader {
public:
    FVulkanShader(FVulkanDevice &Device, const FRHIShaderDescription &Description);
    
    void Shutdown() override;
    
    ERHIShaderType GetType() override { return Type; }
    
public:
    inline VkShaderModule GetVulkanShader() { return Shader; }
    
private:
    FVulkanDevice &Device;
    
    VkShaderModule Shader = VK_NULL_HANDLE;
    ERHIShaderType Type = ERHIShaderType::VERTEX;
};