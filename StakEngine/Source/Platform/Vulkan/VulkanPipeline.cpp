#include "VulkanPipeline.h"

#include "VulkanDescriptorSetLayout.h"
#include "VulkanShader.h"

FVulkanPipelineLayout::FVulkanPipelineLayout(FVulkanDevice &Device, const FRHIPipelineLayoutDescription &Description) : Device(Device) {
    VkPipelineLayoutCreateInfo Info = {};
    Info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    
    std::vector<VkDescriptorSetLayout> Layouts = {};
    Layouts.reserve(Description.DescriptorSetLayouts.size());
    
    for (auto &It : Description.DescriptorSetLayouts) {
        Layouts.push_back(std::static_pointer_cast<FVulkanDescriptorSetLayout>(It.GetImpl())->GetLayout());
    }
    
    Info.setLayoutCount = static_cast<FUInt32>(Layouts.size());
    Info.pSetLayouts = Layouts.data();
    
    CHECK_VK_ERR(vkCreatePipelineLayout(Device.GetVulkanDevice(), &Info, nullptr, &Layout), "Failed to create pipeline layout");
}

void FVulkanPipelineLayout::Shutdown() {
    vkDestroyPipelineLayout(Device.GetVulkanDevice(), Layout, nullptr);
}

FVulkanPipeline::FVulkanPipeline(FVulkanDevice &Device, const FRHIPipelineStateDescription &Description) : Device(Device) {
    std::vector<VkFormat> ColorFormats;
    ColorFormats.reserve(Description.ColorFormats.size());
    
    for (auto &It : Description.ColorFormats) {
        ColorFormats.push_back(GetVulkanFormat(It));
    }
    
    VkPipelineRenderingCreateInfo RenderingInfo = {};
    RenderingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    RenderingInfo.colorAttachmentCount = static_cast<FUInt32>(ColorFormats.size());
    RenderingInfo.pColorAttachmentFormats = ColorFormats.data();
    RenderingInfo.depthAttachmentFormat = GetVulkanDepthFormat(Description.DepthStencilFormat);
    RenderingInfo.stencilAttachmentFormat = GetVulkanStencilFormat(Description.DepthStencilFormat);
    
    std::vector<VkPipelineShaderStageCreateInfo> ShaderStages = {};
    
    for (auto &It : Description.Shaders) {
        VkPipelineShaderStageCreateInfo Info = {};
        Info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        Info.stage = GetVulkanShaderStage(It->GetType());
        Info.module = std::static_pointer_cast<FVulkanShader>(It->GetImpl())->GetVulkanShader();
        Info.pName = "main";
        
        ShaderStages.push_back(Info);
    }
    
    std::vector<VkVertexInputBindingDescription> VertexBindings = {};
    
    for (auto &It : Description.VertexInputBindings) {
        VkVertexInputBindingDescription Binding = {};
        Binding.binding = It.Binding;
        Binding.stride = It.Stride;
        Binding.inputRate = GetVulkanVertexInputRate(It.InputRate);
        VertexBindings.push_back(Binding);
    }
    
    std::vector<VkVertexInputAttributeDescription> VertexAttributes = {};
    
    for (auto &It : Description.VertexInputAttributes) {
        VkVertexInputAttributeDescription Attribute = {};
        Attribute.location = It.Location;
        Attribute.binding = It.Binding;
        Attribute.format = GetVulkanFormat(It.Format);
        Attribute.offset = It.Offset;
        
        VertexAttributes.push_back(Attribute);
    }
    
    VkPipelineVertexInputStateCreateInfo VertexInput = {};
    VertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    VertexInput.vertexBindingDescriptionCount = static_cast<FUInt32>(VertexBindings.size());
    VertexInput.pVertexBindingDescriptions = VertexBindings.data();
    VertexInput.vertexAttributeDescriptionCount = static_cast<FUInt32>(VertexAttributes.size());
    VertexInput.pVertexAttributeDescriptions = VertexAttributes.data();
    
    VkDynamicState DynamicStates[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
        VK_DYNAMIC_STATE_LINE_WIDTH,
        VK_DYNAMIC_STATE_BLEND_CONSTANTS,
        VK_DYNAMIC_STATE_PRIMITIVE_TOPOLOGY,
    };
    
    VkPipelineDynamicStateCreateInfo DynamicState = {};
    DynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    DynamicState.dynamicStateCount = 4;
    DynamicState.pDynamicStates = DynamicStates;
    
    VkGraphicsPipelineCreateInfo Info = {};
    Info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    Info.pNext = &RenderingInfo;
    Info.stageCount = static_cast<FUInt32>(ShaderStages.size());
    Info.pStages = ShaderStages.data();
    Info.pVertexInputState = &VertexInput;
    Info.pDynamicState = &DynamicState;
    Info.layout = std::static_pointer_cast<FVulkanPipelineLayout>(Description.Layout->GetImpl())->GetLayout();
    
    CHECK_VK_ERR(vkCreateGraphicsPipelines(Device.GetVulkanDevice(), VK_NULL_HANDLE, 1, &Info, nullptr, &Pipeline), "Failed to create vulkan pipeline");
}

void FVulkanPipeline::Shutdown() {
    vkDestroyPipeline(Device.GetVulkanDevice(), Pipeline, nullptr);
}
