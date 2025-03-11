#include "VulkanDescriptorSetLayout.h"

#include "VulkanRHI.h"

FVulkanDescriptorSetLayout::FVulkanDescriptorSetLayout(VkDevice Device) : Device(Device) {}

bool FVulkanDescriptorSetLayout::Init(FRHIDescriptorSetLayoutDescription *Description) {
VkDescriptorSetLayoutCreateInfo Info = {};
    Info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;

    std::vector<VkDescriptorSetLayoutBinding> Bindings;

    for (auto &It : Description->Bindings) {
        VkDescriptorSetLayoutBinding Binding = {};
        Binding.binding = It.Binding;
        Binding.descriptorType = VulkanGetDescriptorType(It.Type);
        Binding.descriptorCount = It.Count;
        Binding.stageFlags = VulkanGetShaderStageFlags(It.ShaderStages);

        Bindings.push_back(Binding);
    }

    Info.bindingCount = static_cast<FUInt32>(Bindings.size());
    Info.pBindings = Bindings.data();

    if (vkCreateDescriptorSetLayout(Device, &Info, nullptr, &Layout) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to create descriptor set layout");
        return false;
    }

    return true;
}

void FVulkanDescriptorSetLayout::Shutdown() {
    vkDestroyDescriptorSetLayout(Device, Layout, nullptr);
}
