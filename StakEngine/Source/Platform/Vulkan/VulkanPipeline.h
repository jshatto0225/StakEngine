#pragma once

#include "RHIPipeline.h"
#include "VulkanRHI.h"

class FVulkanPipelineLayout : public IRHIPipelineLayout {
public:
    FVulkanPipelineLayout(VkDevice Device);

    bool Init(const FRHIPipelineLayoutDescription &Description) override;
    void Shutdown() override;

public:
    VkPipelineLayout GetLayout() { return Layout; }

private:
    VkDevice Device;

    VkPipelineLayout Layout = VK_NULL_HANDLE;
};

class FVulkanPipeline : public IRHIPipeline {
public:
    FVulkanPipeline(VkDevice Device);

    bool Init(const FRHIGraphicsPipelineStateDescription &Description) override;
    void Shutdown() override;

public:
    inline VkPipeline GetVulkanPipeline() { return Pipeline; }
    inline VkPipelineBindPoint GetVulkanBindPoint() { return BindPoint; }

private:
    VkDevice Device;

    VkPipelineBindPoint BindPoint;

    VkPipeline Pipeline = VK_NULL_HANDLE;
};
