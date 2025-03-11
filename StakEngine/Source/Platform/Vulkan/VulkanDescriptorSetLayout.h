#pragma once

#include "RHIDescriptorSetLayout.h"
#include "VulkanRHI.h"

class FVulkanDescriptorSetLayout : public IRHIDescriptorSetLayout {
public:
    FVulkanDescriptorSetLayout(VkDevice Device);

    bool Init(FRHIDescriptorSetLayoutDescription *Description) override;
    void Shutdown() override;

public:
    inline VkDescriptorSetLayout GetLayout() const { return Layout; }

private:
    VkDevice Device;

    VkDescriptorSetLayout Layout = VK_NULL_HANDLE;
};
