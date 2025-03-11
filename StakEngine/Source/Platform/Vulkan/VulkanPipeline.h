#pragma once

#include "RHIPipeline.h"
#include "VulkanRHI.h"

struct FVulkanPipelineLayout : public IRHIPipelineLayout {
    FVulkanPipelineLayout(VkDevice Device);

    bool Init(FRHIPipelineLayoutDescription *Description) override;
    void Shutdown() override;

    VkDevice Device;
    VkPipelineLayout Layout = VK_NULL_HANDLE;
};

struct FVulkanPipeline : public IRHIPipeline {
    FVulkanPipeline(VkDevice Device);

    bool Init(FRHIGraphicsPipelineStateDescription *Description) override;
    void Shutdown() override;

    VkDevice Device;
    VkPipelineBindPoint BindPoint;
    VkPipeline Pipeline = VK_NULL_HANDLE;
};
