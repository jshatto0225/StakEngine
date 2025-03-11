#include "VulkanShader.h"

#include <fstream>

FVulkanShader::FVulkanShader(VkDevice Device) : Device(Device) {

}

bool FVulkanShader::Init(FRHIShaderDescription *Description) {
    Type = Description->Type;
    std::string Path = "Assets/Shaders/" + Description->Name + ".spv";
    std::ifstream File(Path, std::ios::ate | std::ios::binary);

    if (!File.is_open()) {
        SK_LOG_ERROR("Failed to open shader file");
        return false;
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

    if (vkCreateShaderModule(Device, &Info, nullptr, &Shader) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to create shader module");
        return false;
    }

    return true;
}

void FVulkanShader::Shutdown() {
    vkDestroyShaderModule(Device, Shader, nullptr);
}
