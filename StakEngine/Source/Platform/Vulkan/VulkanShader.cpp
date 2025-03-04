#include "VulkanShader.h"

#include <fstream>

FVulkanShader::FVulkanShader(FVulkanDevice &Device, const FRHIShaderDescription &Description) : Type(Description.Type), Device(Device) {
    std::string Path = "Assets/Shaders/" + Description.Name + ".spv";
    std::ifstream File(Path, std::ios::ate | std::ios::binary);
    
    if (!File.is_open()) {
        SK_LOG_ERROR("Failed to open shader file");
        return;
    }
    
    FUInt32 FileSize = static_cast<FUInt32>(File.tellg());
    std::vector<char> ShaderCode(FileSize);
    File.seekg(0);
    File.read(ShaderCode.data(), FileSize);
    File.close();
    
    VkShaderModuleCreateInfo Info = {};
    Info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    Info.codeSize = static_cast<FUInt32>(ShaderCode.size());
    Info.pCode = reinterpret_cast<const FUInt32 *>(ShaderCode.data());
    
    CHECK_VK_ERR(vkCreateShaderModule(Device.GetVulkanDevice(), &Info, nullptr, &Shader), "Failed to create shader module");
}

void FVulkanShader::Shutdown() {
    vkDestroyShaderModule(Device.GetVulkanDevice(), Shader, nullptr);
}