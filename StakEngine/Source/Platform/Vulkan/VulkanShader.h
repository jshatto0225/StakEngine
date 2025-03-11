#pragma once

#include "RHIShader.h"

#include "VulkanRHI.h"

struct FVulkanShader : public IRHIShader {
    FVulkanShader(VkDevice Device);

    bool Init(FRHIShaderDescription *Description) override;
    void Shutdown() override;

    VkDevice Device;
    VkShaderModule Shader = VK_NULL_HANDLE;
};
