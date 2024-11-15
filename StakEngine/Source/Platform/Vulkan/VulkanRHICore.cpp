#include "VulkanRHICore.h"

VkFormat VulkanRHIGetVkFormat(ERHIFormat Format) {
  switch (Format) {
  case ERHIFormat::R8_SINT:
    return VK_FORMAT_R8_SINT;
  case ERHIFormat::R8_UINT:
    return VK_FORMAT_R8_UINT;
  case ERHIFormat::R8_UNORM:
    return VK_FORMAT_R8_UNORM;
  case ERHIFormat::R8_SNORM:
    return VK_FORMAT_R8_SNORM;
  case ERHIFormat::R8_SRGB:
    return VK_FORMAT_R8_SRGB;
  case ERHIFormat::R8G8_SINT:
    return VK_FORMAT_R8G8_SINT;
  case ERHIFormat::R8G8_UINT:
    return VK_FORMAT_R8G8_UINT;
  case ERHIFormat::R8G8_UNORM:
    return VK_FORMAT_R8G8_UNORM;
  case ERHIFormat::R8G8_SNORM:
    return VK_FORMAT_R8G8_SNORM;
  case ERHIFormat::R8G8_SRGB:
    return VK_FORMAT_R8G8_SRGB;
  case ERHIFormat::R8G8B8_SINT:
    return VK_FORMAT_R8G8B8_SINT;
  case ERHIFormat::R8G8B8_UINT:
    return VK_FORMAT_R8G8B8_UINT;
  case ERHIFormat::R8G8B8_UNORM:
    return VK_FORMAT_R8G8B8_UNORM;
  case ERHIFormat::R8G8B8_SNORM:
    return VK_FORMAT_R8G8B8_SNORM;
  case ERHIFormat::R8G8B8_SRGB:
    return VK_FORMAT_R8G8B8_SRGB;
  case ERHIFormat::R8G8B8A8_SINT:
    return VK_FORMAT_R8G8B8A8_SINT;
  case ERHIFormat::R8G8B8A8_UINT:
    return VK_FORMAT_R8G8B8A8_UINT;
  case ERHIFormat::R8G8B8A8_UNORM:
    return VK_FORMAT_R8G8B8A8_UNORM;
  case ERHIFormat::R8G8B8A8_SNORM:
    return VK_FORMAT_R8G8B8A8_SNORM;
  case ERHIFormat::R8G8B8A8_SRGB:
    return VK_FORMAT_R8G8B8A8_SRGB;
  case ERHIFormat::R16_SINT:
    return VK_FORMAT_R16_SINT;
  case ERHIFormat::R16_UINT:
    return VK_FORMAT_R16_UINT;
  case ERHIFormat::R16_FLOAT:
    return VK_FORMAT_R16_SFLOAT;
  case ERHIFormat::R16_UNORM:
    return VK_FORMAT_R16_UNORM;
  case ERHIFormat::R16_SNORM:
    return VK_FORMAT_R16_SNORM;
  case ERHIFormat::R16G16_SINT:
    return VK_FORMAT_R16G16_SINT;
  case ERHIFormat::R16G16_UINT:
    return VK_FORMAT_R16G16_UINT;
  case ERHIFormat::R16G16_FLOAT:
    return VK_FORMAT_R16G16_SFLOAT;
  case ERHIFormat::R16G16_UNORM:
    return VK_FORMAT_R16G16_UNORM;
  case ERHIFormat::R16G16_SNORM:
    return VK_FORMAT_R16G16_SNORM;
  case ERHIFormat::R16G16B16_SINT:
    return VK_FORMAT_R16G16B16_SINT;
  case ERHIFormat::R16G16B16_UINT:
    return VK_FORMAT_R16G16B16_UINT;
  case ERHIFormat::R16G16B16_FLOAT:
    return VK_FORMAT_R16G16B16_SFLOAT;
  case ERHIFormat::R16G16B16_UNORM:
    return VK_FORMAT_R16G16B16_UNORM;
  case ERHIFormat::R16G16B16_SNORM:
    return VK_FORMAT_R16G16B16_SNORM;
  case ERHIFormat::R16G16B16A16_SINT:
    return VK_FORMAT_R16G16B16A16_SINT;
  case ERHIFormat::R16G16B16A16_UINT:
    return VK_FORMAT_R16G16B16A16_UINT;
  case ERHIFormat::R16G16B16A16_FLOAT:
    return VK_FORMAT_R16G16B16A16_SFLOAT;
  case ERHIFormat::R16G16B16A16_UNORM:
    return VK_FORMAT_R16G16B16A16_UNORM;
  case ERHIFormat::R16G16B16A16_SNORM:
    return VK_FORMAT_R16G16B16A16_SNORM;
  case ERHIFormat::R32_SINT:
    return VK_FORMAT_R32_SINT;
  case ERHIFormat::R32_UINT:
    return VK_FORMAT_R32_UINT;
  case ERHIFormat::R32_FLOAT:
    return VK_FORMAT_R32_SFLOAT;
  case ERHIFormat::R32G32_SINT:
    return VK_FORMAT_R32G32_SINT;
  case ERHIFormat::R32G32_UINT:
    return VK_FORMAT_R32G32_UINT;
  case ERHIFormat::R32G32_FLOAT:
    return VK_FORMAT_R32G32_SFLOAT;
  case ERHIFormat::R32G32B32_SINT:
    return VK_FORMAT_R32G32B32_SINT;
  case ERHIFormat::R32G32B32_UINT:
    return VK_FORMAT_R32G32B32_UINT;
  case ERHIFormat::R32G32B32_FLOAT:
    return VK_FORMAT_R32G32B32_SFLOAT;
  case ERHIFormat::R32G32B32A32_SINT:
    return VK_FORMAT_R32G32B32A32_SINT;
  case ERHIFormat::R32G32B32A32_UINT:
    return VK_FORMAT_R32G32B32A32_UINT;
  case ERHIFormat::R32G32B32A32_FLOAT:
    return VK_FORMAT_R32G32B32A32_SFLOAT;
  case ERHIFormat::R64_SINT:
    return VK_FORMAT_R64_SINT;
  case ERHIFormat::R64_UINT:
    return VK_FORMAT_R64_UINT;
  case ERHIFormat::R64_FLOAT:
    return VK_FORMAT_R64_SFLOAT;
  case ERHIFormat::R64G64_SINT:
    return VK_FORMAT_R64G64_SINT;
  case ERHIFormat::R64G64_UINT:
    return VK_FORMAT_R64G64_UINT;
  case ERHIFormat::R64G64_FLOAT:
    return VK_FORMAT_R64G64_SFLOAT;
  case ERHIFormat::R64G64B64_SINT:
    return VK_FORMAT_R64G64B64_SINT;
  case ERHIFormat::R64G64B64_UINT:
    return VK_FORMAT_R64G64B64_UINT;
  case ERHIFormat::R64G64B64_FLOAT:
    return VK_FORMAT_R64G64B64_SFLOAT;
  case ERHIFormat::R64G64B64A64_SINT:
    return VK_FORMAT_R64G64B64A64_SINT;
  case ERHIFormat::R64G64B64A64_UINT:
    return VK_FORMAT_R64G64B64A64_UINT;
  case ERHIFormat::R64G64B64A64_FLOAT:
    return VK_FORMAT_R64G64B64A64_SFLOAT;
  case ERHIFormat::D32_FLOAT:
    return VK_FORMAT_D32_SFLOAT;
  case ERHIFormat::D24_UNORM_S8_UINT:
    return VK_FORMAT_D24_UNORM_S8_UINT;
  case ERHIFormat::S8_UINT:
    return VK_FORMAT_S8_UINT;
  }
}

VkSampleCountFlagBits VulkanRHIGetVkSampleCount(ERHISampleCount SampleCount) {
  switch (SampleCount) {
    case ERHISampleCount::ONE:
      return VK_SAMPLE_COUNT_1_BIT;
    case ERHISampleCount::TWO:
      return VK_SAMPLE_COUNT_2_BIT;
    case ERHISampleCount::FOUR:
      return VK_SAMPLE_COUNT_4_BIT;
    case ERHISampleCount::EIGHT:
      return VK_SAMPLE_COUNT_8_BIT;
    case ERHISampleCount::SIXTEEN:
      return VK_SAMPLE_COUNT_16_BIT;
    case ERHISampleCount::THIRTY_TWO:
      return VK_SAMPLE_COUNT_32_BIT;
    case ERHISampleCount::SIXTY_FOUR:
      return VK_SAMPLE_COUNT_64_BIT;
  }
}

VkAttachmentLoadOp VulkanRHIGetVkLoadOp(ERHILoadOp LoadOp) {
  switch (LoadOp) {
  case ERHILoadOp::LOAD:
    return VK_ATTACHMENT_LOAD_OP_LOAD;
  case ERHILoadOp::CLEAR:
    return VK_ATTACHMENT_LOAD_OP_CLEAR;
  case ERHILoadOp::DISCARD:
    return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  }
}

VkAttachmentStoreOp VulkanRHIGetVkStoreOp(ERHIStoreOp StoreOp) {
  switch (StoreOp) {
  case ERHIStoreOp::STORE:
    return VK_ATTACHMENT_STORE_OP_STORE;
  case ERHIStoreOp::DISCARD:
    return VK_ATTACHMENT_STORE_OP_DONT_CARE;
  }
}

VkImageLayout VulkanRHIGetVkImageLayout(ERHIImageState State) {
  switch (State) {
  case ERHIImageState::COLOR_ATTACHMENT:
    return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  case ERHIImageState::DEPTH_STENCIL:
    return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
  case ERHIImageState::PRESENT:
    return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  case ERHIImageState::SHADER_READ:
    return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  case ERHIImageState::TRANSFER_DESTINATION:
    return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
  case ERHIImageState::TRANSFER_SOURCE:
    return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
  case ERHIImageState::UNDEFINED:
    return VK_IMAGE_LAYOUT_UNDEFINED;
  }
}

VkPipelineBindPoint VulkanRHIGetVkPipelineBindPoint(ERHIPipelineBindPoint BindPoint) {
  switch (BindPoint) {
  case ERHIPipelineBindPoint::COMPUTE:
    return VK_PIPELINE_BIND_POINT_COMPUTE;
  case ERHIPipelineBindPoint::GRAPHICS:
    return VK_PIPELINE_BIND_POINT_GRAPHICS;
  }
}

VkPipelineStageFlags VulkanRHIGetVkPipelineStage(ERHIPipelineStage Stage) {
  switch (Stage) {
  case ERHIPipelineStage::TOP_OF_PIPE:
    return VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
  case ERHIPipelineStage::VERTEX_INPUT:
    return VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
  case ERHIPipelineStage::VERTEX_SHADER:
    return VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
  case ERHIPipelineStage::TESSELLATION_CONTROL_SHADER:
    return VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT;
  case ERHIPipelineStage::TESSELLATION_EVALUATION_SHADER:
    return VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT;
  case ERHIPipelineStage::GEOMETRY_SHADER:
    return VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT;
  case ERHIPipelineStage::FRAGMENT_SHADER:
    return VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
  case ERHIPipelineStage::FRAMEBUFFER_WRITE:
    return VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  case ERHIPipelineStage::COMPUTE_SHADER:
    return VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
  case ERHIPipelineStage::BOTTOM_OF_PIPE:
    return VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
  }
}

VkAccessFlags VulkanRHIGetVkAccessMask(ERHIAccess Access) {
  switch (Access) {
  case ERHIAccess::COLOR_ATTACHMENT_READ:
    return VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
  case ERHIAccess::COLOR_ATTACHMENT_WRITE:
    return VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
  case ERHIAccess::DEPTH_STENCIL_ATTACHMENT_READ:
    return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
  case ERHIAccess::DEPTH_STENCIL_ATTACHMENT_WRITE:
    return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
  case ERHIAccess::HOST_READ:
    return VK_ACCESS_HOST_READ_BIT;
  case ERHIAccess::HOST_WRITE:
    return VK_ACCESS_HOST_WRITE_BIT;
  case ERHIAccess::INDEX_READ:
    return VK_ACCESS_INDEX_READ_BIT;
  case ERHIAccess::INPUT_ATTACHMENT_READ:
    return VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
  case ERHIAccess::MEMORY_READ:
    return VK_ACCESS_MEMORY_READ_BIT;
  }
}
