#include "VulkanDescriptorSetLayout.h"

#include "VulkanRHI.h"

FVulkanDescriptorSetLayout::FVulkanDescriptorSetLayout(FVulkanDevice &Device, const FRHIDescriptorSetLayoutDescription &Description) : Device(Device) {
    VkDescriptorSetLayoutCreateInfo Info = {};
    Info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    
    std::vector<VkDescriptorSetLayoutBinding> Bindings;
    
    for (auto &It : Description.Bindings) {
        VkDescriptorSetLayoutBinding Binding = {};
        Binding.binding = It.Binding;
        Binding.descriptorType = GetVulkanDescriptorType(It.Type);
        Binding.descriptorCount = It.Count;
        Binding.stageFlags = GetVulkanShaderStageFlags(It.ShaderStages);
        
        Bindings.push_back(Binding);
    }
    
    Info.bindingCount = static_cast<FUInt32>(Bindings.size());
    Info.pBindings = Bindings.data();
        
    CHECK_VK_ERR(vkCreateDescriptorSetLayout(Device.GetVulkanDevice(), &Info, nullptr, &Layout), "Failed to create descriptor set layout");
}

void FVulkanDescriptorSetLayout::Shutdown() {
    vkDestroyDescriptorSetLayout(Device.GetVulkanDevice(), Layout, nullptr);
}
    