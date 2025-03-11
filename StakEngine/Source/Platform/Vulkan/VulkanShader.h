#pragma once

#include "RHIShader.h"

#include "VulkanRHI.h"

class FVulkanShader : public IRHIShader {
public:
    FVulkanShader(VkDevice Device);

    bool Init(FRHIShaderDescription *Description) override;
    void Shutdown() override;

    ERHIShaderType GetType() override { return Type; }

public:
    inline VkShaderModule GetVulkanShader() { return Shader; }

private:
    VkDevice Device;

    VkShaderModule Shader = VK_NULL_HANDLE;
    ERHIShaderType Type = ERHIShaderType::VERTEX;
};
