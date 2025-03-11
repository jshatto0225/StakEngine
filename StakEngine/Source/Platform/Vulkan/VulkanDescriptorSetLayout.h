#pragma once

#include "RHIDescriptorSetLayout.h"
#include "VulkanRHI.h"

struct FVulkanDescriptorSetLayout : public IRHIDescriptorSetLayout {
    FVulkanDescriptorSetLayout(VkDevice Device);

    bool Init(FRHIDescriptorSetLayoutDescription *Description) override;
    void Shutdown() override;

    VkDevice Device;
    VkDescriptorSetLayout Layout = VK_NULL_HANDLE;
};
