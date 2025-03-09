#include "VulkanPipeline.h"

#include "VulkanDescriptorSetLayout.h"
#include "VulkanShader.h"

FVulkanPipelineLayout::FVulkanPipelineLayout(VkDevice Device) : Device(Device) {}

bool FVulkanPipelineLayout::Init(const FRHIPipelineLayoutDescription &Description) {
VkPipelineLayoutCreateInfo Info = {};
    Info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

    std::vector<VkDescriptorSetLayout> Layouts = {};
    Layouts.reserve(Description.DescriptorSetLayouts.size());

    for (auto &It : Description.DescriptorSetLayouts) {
        Layouts.push_back(std::static_pointer_cast<FVulkanDescriptorSetLayout>(It)->GetLayout());
    }

    Info.setLayoutCount = static_cast<FUInt32>(Layouts.size());
    Info.pSetLayouts = Layouts.data();

    if (vkCreatePipelineLayout(Device, &Info, nullptr, &Layout) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to create pipeline layout");
        return false;
    }

    return true;
}

void FVulkanPipelineLayout::Shutdown() {
    vkDestroyPipelineLayout(Device, Layout, nullptr);
}

FVulkanPipeline::FVulkanPipeline(VkDevice Device) : Device(Device), BindPoint(VK_PIPELINE_BIND_POINT_GRAPHICS) {}

bool FVulkanPipeline::Init(const FRHIGraphicsPipelineStateDescription &Description) {
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
    RenderingInfo.viewMask = 0x01;

    std::vector<VkPipelineShaderStageCreateInfo> ShaderStages = {};

    for (auto &It : Description.Shaders) {
        VkPipelineShaderStageCreateInfo Info = {};
        Info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        Info.stage = GetVulkanShaderStage(It->GetType());
        Info.module = std::static_pointer_cast<FVulkanShader>(It)->GetVulkanShader();
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

    VkPipelineInputAssemblyStateCreateInfo InputAssembly = {};
    InputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    InputAssembly.primitiveRestartEnable = false;
    InputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    VkPipelineTessellationStateCreateInfo Tesselation = {};

    VkPipelineViewportStateCreateInfo Viewport = {};
    Viewport.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    Viewport.viewportCount = 1;
    Viewport.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo Rasterizer = {};
    Rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    Rasterizer.depthClampEnable = VK_FALSE;
    Rasterizer.rasterizerDiscardEnable = VK_FALSE;
    Rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    Rasterizer.lineWidth = 1.0f;
    Rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    Rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    Rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo Multisampling = {};
    Multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    Multisampling.sampleShadingEnable = VK_FALSE;
    Multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkDynamicState DynamicStates[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
    };

    VkPipelineDynamicStateCreateInfo DynamicState = {};
    DynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    DynamicState.dynamicStateCount = 2;
    DynamicState.pDynamicStates = DynamicStates;

    VkPipelineColorBlendAttachmentState ColorBlendAttachment = {};
    ColorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    ColorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo ColorBlending = {};
    ColorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    ColorBlending.logicOpEnable = VK_FALSE;
    ColorBlending.logicOp = VK_LOGIC_OP_COPY;
    ColorBlending.attachmentCount = 1;
    ColorBlending.pAttachments = &ColorBlendAttachment;
    ColorBlending.blendConstants[0] = 0.0f;
    ColorBlending.blendConstants[1] = 0.0f;
    ColorBlending.blendConstants[2] = 0.0f;
    ColorBlending.blendConstants[3] = 0.0f;

    VkGraphicsPipelineCreateInfo Info = {};
    Info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    Info.pNext = &RenderingInfo;
    Info.stageCount = static_cast<FUInt32>(ShaderStages.size());
    Info.pStages = ShaderStages.data();
    Info.pVertexInputState = &VertexInput;
    Info.pInputAssemblyState = &InputAssembly;
    Info.pTessellationState = &Tesselation;
    Info.pViewportState = &Viewport;
    Info.pRasterizationState = &Rasterizer;
    Info.pMultisampleState = &Multisampling;
    Info.pDepthStencilState = nullptr;
    Info.pColorBlendState = &ColorBlending;
    Info.pDynamicState = &DynamicState;
    Info.layout = std::static_pointer_cast<FVulkanPipelineLayout>(Description.Layout)->GetLayout();

    if (vkCreateGraphicsPipelines(Device, VK_NULL_HANDLE, 1, &Info, nullptr, &Pipeline) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to create vulkan pipeline");
        return false;
    }

    return true;
}

void FVulkanPipeline::Shutdown() {
    vkDestroyPipeline(Device, Pipeline, nullptr);
}
