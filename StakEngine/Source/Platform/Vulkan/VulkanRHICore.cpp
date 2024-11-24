#include "VulkanRHICore.h"

#include "FLog.h"

ERHIFormat VulkanRHIGetERHIFormat(VkFormat Format) {
  switch (Format) {
  case VK_FORMAT_R8_SINT:
    return ERHIFormat::R8_SINT;
  case VK_FORMAT_R8_UINT:
    return ERHIFormat::R8_UINT;
  case VK_FORMAT_R8_UNORM:
    return ERHIFormat::R8_UNORM;
  case VK_FORMAT_R8_SNORM:
    return ERHIFormat::R8_SNORM;
  case VK_FORMAT_R8_SRGB:
    return ERHIFormat::R8_SRGB;
  case VK_FORMAT_R8G8_SINT:
    return ERHIFormat::R8G8_SINT;
  case VK_FORMAT_R8G8_UINT:
    return ERHIFormat::R8G8_UINT;
  case VK_FORMAT_R8G8_UNORM:
    return ERHIFormat::R8G8_UNORM;
  case VK_FORMAT_R8G8_SNORM:
    return ERHIFormat::R8G8_SNORM;
  case VK_FORMAT_R8G8_SRGB:
    return ERHIFormat::R8G8_SRGB;
  case VK_FORMAT_R8G8B8_SINT:
    return ERHIFormat::R8G8B8_SINT;
  case VK_FORMAT_R8G8B8_UINT:
    return ERHIFormat::R8G8B8_UINT;
  case VK_FORMAT_R8G8B8_UNORM:
    return ERHIFormat::R8G8B8_UNORM;
  case VK_FORMAT_R8G8B8_SNORM:
    return ERHIFormat::R8G8B8_SNORM;
  case VK_FORMAT_R8G8B8_SRGB:
    return ERHIFormat::R8G8B8_SRGB;
  case VK_FORMAT_R8G8B8A8_SINT:
    return ERHIFormat::R8G8B8A8_SINT;
  case VK_FORMAT_R8G8B8A8_UINT:
    return ERHIFormat::R8G8B8A8_UINT;
  case VK_FORMAT_R8G8B8A8_UNORM:
    return ERHIFormat::R8G8B8A8_UNORM;
  case VK_FORMAT_R8G8B8A8_SNORM:
    return ERHIFormat::R8G8B8A8_SNORM;
  case VK_FORMAT_R8G8B8A8_SRGB:
    return ERHIFormat::R8G8B8A8_SRGB;
  case VK_FORMAT_R16_SINT:
    return ERHIFormat::R16_SINT;
  case VK_FORMAT_R16_UINT:
    return ERHIFormat::R16_UINT;
  case VK_FORMAT_R16_SFLOAT:
    return ERHIFormat::R16_FLOAT;
  case VK_FORMAT_R16_UNORM:
    return ERHIFormat::R16_UNORM;
  case VK_FORMAT_R16_SNORM:
    return ERHIFormat::R16_SNORM;
  case VK_FORMAT_R16G16_SINT:
    return ERHIFormat::R16G16_SINT;
  case VK_FORMAT_R16G16_UINT:
    return ERHIFormat::R16G16_UINT;
  case VK_FORMAT_R16G16_SFLOAT:
    return ERHIFormat::R16G16_FLOAT;
  case VK_FORMAT_R16G16_UNORM:
    return ERHIFormat::R16G16_UNORM;
  case VK_FORMAT_R16G16_SNORM:
    return ERHIFormat::R16G16_SNORM;
  case VK_FORMAT_R16G16B16_SINT:
    return ERHIFormat::R16G16B16_SINT;
  case VK_FORMAT_R16G16B16_UINT:
    return ERHIFormat::R16G16B16_UINT;
  case VK_FORMAT_R16G16B16_SFLOAT:
    return ERHIFormat::R16G16B16_FLOAT;
  case VK_FORMAT_R16G16B16_UNORM:
    return ERHIFormat::R16G16B16_UNORM;
  case VK_FORMAT_R16G16B16_SNORM:
    return ERHIFormat::R16G16B16_SNORM;
  case VK_FORMAT_R16G16B16A16_SINT:
    return ERHIFormat::R16G16B16A16_SINT;
  case VK_FORMAT_R16G16B16A16_UINT:
    return ERHIFormat::R16G16B16A16_UINT;
  case VK_FORMAT_R16G16B16A16_SFLOAT:
    return ERHIFormat::R16G16B16A16_FLOAT;
  case VK_FORMAT_R16G16B16A16_UNORM:
    return ERHIFormat::R16G16B16A16_UNORM;
  case VK_FORMAT_R16G16B16A16_SNORM:
    return ERHIFormat::R16G16B16A16_SNORM;
  case VK_FORMAT_R32_SINT:
    return ERHIFormat::R32_SINT;
  case VK_FORMAT_R32_UINT:
    return ERHIFormat::R32_UINT;
  case VK_FORMAT_R32_SFLOAT:
    return ERHIFormat::R32_FLOAT;
  case VK_FORMAT_R32G32_SINT:
    return ERHIFormat::R32G32_SINT;
  case VK_FORMAT_R32G32_UINT:
    return ERHIFormat::R32G32_UINT;
  case VK_FORMAT_R32G32_SFLOAT:
    return ERHIFormat::R32G32_FLOAT;
  case VK_FORMAT_R32G32B32_SINT:
    return ERHIFormat::R32G32B32_SINT;
  case VK_FORMAT_R32G32B32_UINT:
    return ERHIFormat::R32G32B32_UINT;
  case VK_FORMAT_R32G32B32_SFLOAT:
    return ERHIFormat::R32G32B32_FLOAT;
  case VK_FORMAT_R32G32B32A32_SINT:
    return ERHIFormat::R32G32B32A32_SINT;
  case VK_FORMAT_R32G32B32A32_UINT:
    return ERHIFormat::R32G32B32A32_UINT;
  case VK_FORMAT_R32G32B32A32_SFLOAT:
    return ERHIFormat::R32G32B32A32_FLOAT;
  case VK_FORMAT_R64_SINT:
    return ERHIFormat::R64_SINT;
  case VK_FORMAT_R64_UINT:
    return ERHIFormat::R64_UINT;
  case VK_FORMAT_R64_SFLOAT:
    return ERHIFormat::R64_FLOAT;
  case VK_FORMAT_R64G64_SINT:
    return ERHIFormat::R64G64_SINT;
  case VK_FORMAT_R64G64_UINT:
    return ERHIFormat::R64G64_UINT;
  case VK_FORMAT_R64G64_SFLOAT:
    return ERHIFormat::R64G64_FLOAT;
  case VK_FORMAT_R64G64B64_SINT:
    return ERHIFormat::R64G64B64_SINT;
  case VK_FORMAT_R64G64B64_UINT:
    return ERHIFormat::R64G64B64_UINT;
  case VK_FORMAT_R64G64B64_SFLOAT:
    return ERHIFormat::R64G64B64_FLOAT;
  case VK_FORMAT_R64G64B64A64_SINT:
    return ERHIFormat::R64G64B64A64_SINT;
  case VK_FORMAT_R64G64B64A64_UINT:
    return ERHIFormat::R64G64B64A64_UINT;
  case VK_FORMAT_R64G64B64A64_SFLOAT:
    return ERHIFormat::R64G64B64A64_FLOAT;
  case VK_FORMAT_D32_SFLOAT:
    return ERHIFormat::D32_FLOAT;
  case VK_FORMAT_D24_UNORM_S8_UINT:
    return ERHIFormat::D24_UNORM_S8_UINT;
  case VK_FORMAT_S8_UINT:
    return ERHIFormat::S8_UINT;
  case VK_FORMAT_B8G8R8A8_UNORM:
    return ERHIFormat::B8G8R8A8_UNORM;
  case VK_FORMAT_B8G8R8A8_SRGB:
    return ERHIFormat::B8G8R8A8_UNORM_SRGB;
  default:
    SK_LOG_ERROR("Unsupported vulkan format");
  }
}

VkFormat VulkanRHIGetVkFormat(ERHIFormat Format) {
  switch (Format) {
  case ERHIFormat::B8G8R8A8_UNORM:
    return VK_FORMAT_B8G8R8A8_UNORM;
  case ERHIFormat::B8G8R8A8_UNORM_SRGB:
    return VK_FORMAT_B8G8R8A8_SRGB;
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
  default:
    SK_LOG_ERROR("Unsupported rhi format");
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
    default:
      SK_LOG_ERROR("Unsupported sample count");
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
  default:
    SK_LOG_ERROR("Unsupported load op");
  }
}

VkAttachmentStoreOp VulkanRHIGetVkStoreOp(ERHIStoreOp StoreOp) {
  switch (StoreOp) {
  case ERHIStoreOp::STORE:
    return VK_ATTACHMENT_STORE_OP_STORE;
  case ERHIStoreOp::DISCARD:
    return VK_ATTACHMENT_STORE_OP_DONT_CARE;
  default:
    SK_LOG_ERROR("Unsupported store op");
  }
}

VkPipelineBindPoint VulkanRHIGetVkPipelineBindPoint(ERHIPipelineBindPoint BindPoint) {
  switch (BindPoint) {
  case ERHIPipelineBindPoint::COMPUTE:
    return VK_PIPELINE_BIND_POINT_COMPUTE;
  case ERHIPipelineBindPoint::GRAPHICS:
    return VK_PIPELINE_BIND_POINT_GRAPHICS;
  default:
    SK_LOG_ERROR("Unsupported pipeline bind point");
  }
}

VkClearValue VulkanRHIGetVkClearValue(FRHIClearValue ClearValue) {
  VkClearValue VulkanClearValue = {};
  VulkanClearValue.color.float32[0] = ClearValue.Color[0];
  VulkanClearValue.color.float32[1] = ClearValue.Color[1];
  VulkanClearValue.color.float32[2] = ClearValue.Color[2];
  VulkanClearValue.color.float32[3] = ClearValue.Color[3];

  VulkanClearValue.depthStencil.depth = ClearValue.DepthStencil.Depth;
  VulkanClearValue.depthStencil.stencil = ClearValue.DepthStencil.Stencil;

  return VulkanClearValue;
}

VkPrimitiveTopology VulkanRHIGetVkTopology(ERHITopology Topology) {
  switch (Topology) {
  case ERHITopology::POINT:
    return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
  case ERHITopology::LINE:
    return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
  case ERHITopology::TRIANGLE:
    return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  case ERHITopology::PATCH:
    return VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
  default:
    SK_LOG_ERROR("Unsupported topology");
  }
}

VkIndexType VulkanRHIGetVkIndexType(FUInt32 Stride) {
  switch (Stride) {
  case 2:
    return VK_INDEX_TYPE_UINT16;
  case 4:
    return VK_INDEX_TYPE_UINT16;
  default:
    SK_LOG_ERROR("Unsupported index size");
  }
}

VkImageLayout VulkanRHIGetVkImageLayout(ERHIResourceUsage Usage) {
  switch (Usage) {
  case ERHIResourceUsage::RESOURCE_STATE_UNDEFINED:
    return VK_IMAGE_LAYOUT_UNDEFINED;
  case ERHIResourceUsage::RESOURCE_STATE_VERTEX_BUFFER:
    return VK_IMAGE_LAYOUT_UNDEFINED;
  case ERHIResourceUsage::RESOURCE_STATE_CONSTANT_BUFFER:
    return VK_IMAGE_LAYOUT_UNDEFINED;
  case ERHIResourceUsage::RESOURCE_STATE_INDEX_BUFFER:
    return VK_IMAGE_LAYOUT_UNDEFINED;
  case ERHIResourceUsage::RESOURCE_STATE_INDIRECT_ARGUMENT:
    return VK_IMAGE_LAYOUT_UNDEFINED;
  case ERHIResourceUsage::RESOURCE_STATE_COPY_SOURCE:
    return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
  case ERHIResourceUsage::RESOURCE_STATE_COPY_DEST:
    return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
  case ERHIResourceUsage::RESOURCE_STATE_SHADER_RESOURCE:
    return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  case ERHIResourceUsage::RESOURCE_STATE_UNORDERED_ACCESS:
    return VK_IMAGE_LAYOUT_GENERAL;
  case ERHIResourceUsage::RESOURCE_STATE_RENDER_TARGET:
    return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  case ERHIResourceUsage::RESOURCE_STATE_DEPTH_WRITE:
    return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
  case ERHIResourceUsage::RESOURCE_STATE_DEPTH_READ:
    return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
  case ERHIResourceUsage::RESOURCE_STATE_PRESENT:
    return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  case ERHIResourceUsage::RESOURCE_STATE_GENERIC_READ:
    return VK_IMAGE_LAYOUT_UNDEFINED;
  default:
    SK_LOG_ERROR("Unsupported Resource Usage");
    return VK_IMAGE_LAYOUT_UNDEFINED;
  }
}

VkAccessFlagBits2 VulkanRHIGetVkAccessFlagBits2(ERHIResourceUsage Usage) {
  switch (Usage) {
  case ERHIResourceUsage::RESOURCE_STATE_UNDEFINED:
    return 0;
  case ERHIResourceUsage::RESOURCE_STATE_VERTEX_BUFFER:
    return VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT;
  case ERHIResourceUsage::RESOURCE_STATE_CONSTANT_BUFFER:
    return VK_ACCESS_2_UNIFORM_READ_BIT;
  case ERHIResourceUsage::RESOURCE_STATE_INDEX_BUFFER:
    return VK_ACCESS_2_INDEX_READ_BIT;
  case ERHIResourceUsage::RESOURCE_STATE_INDIRECT_ARGUMENT:
    return VK_ACCESS_2_INDIRECT_COMMAND_READ_BIT;
  case ERHIResourceUsage::RESOURCE_STATE_COPY_SOURCE:
    return VK_ACCESS_2_TRANSFER_READ_BIT;
  case ERHIResourceUsage::RESOURCE_STATE_COPY_DEST:
    return VK_ACCESS_2_TRANSFER_WRITE_BIT;
  case ERHIResourceUsage::RESOURCE_STATE_SHADER_RESOURCE:
    return VK_ACCESS_2_SHADER_READ_BIT;
  case ERHIResourceUsage::RESOURCE_STATE_UNORDERED_ACCESS:
    return VK_ACCESS_2_SHADER_READ_BIT | VK_ACCESS_2_SHADER_WRITE_BIT;
  case ERHIResourceUsage::RESOURCE_STATE_RENDER_TARGET:
    return VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
  case ERHIResourceUsage::RESOURCE_STATE_DEPTH_WRITE:
    return VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
  case ERHIResourceUsage::RESOURCE_STATE_DEPTH_READ:
    return VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
  case ERHIResourceUsage::RESOURCE_STATE_PRESENT:
    return 0;
  case ERHIResourceUsage::RESOURCE_STATE_GENERIC_READ:
    return VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT | VK_ACCESS_2_UNIFORM_READ_BIT | VK_ACCESS_2_SHADER_READ_BIT;
  default:
    SK_LOG_ERROR("Unsupported Resource Usage");
    return 0;
  }
}

VkPipelineStageFlags2 VulkanRHIGetVkPipelineStageFlags2(ERHIResourceUsage Usage) {
  switch (Usage) {
  case ERHIResourceUsage::RESOURCE_STATE_UNDEFINED:
    return VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
  case ERHIResourceUsage::RESOURCE_STATE_VERTEX_BUFFER:
    return VK_PIPELINE_STAGE_2_VERTEX_INPUT_BIT;
  case ERHIResourceUsage::RESOURCE_STATE_CONSTANT_BUFFER:
    return VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT | VK_PIPELINE_STAGE_2_GEOMETRY_SHADER_BIT;
  case ERHIResourceUsage::RESOURCE_STATE_INDEX_BUFFER:
    return VK_PIPELINE_STAGE_2_VERTEX_INPUT_BIT;
  case ERHIResourceUsage::RESOURCE_STATE_INDIRECT_ARGUMENT:
    return VK_PIPELINE_STAGE_2_DRAW_INDIRECT_BIT;
  case ERHIResourceUsage::RESOURCE_STATE_COPY_SOURCE:
    return VK_PIPELINE_STAGE_2_TRANSFER_BIT;
  case ERHIResourceUsage::RESOURCE_STATE_COPY_DEST:
    return VK_PIPELINE_STAGE_2_TRANSFER_BIT;
  case ERHIResourceUsage::RESOURCE_STATE_SHADER_RESOURCE:
    return VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
  case ERHIResourceUsage::RESOURCE_STATE_UNORDERED_ACCESS:
    return VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT | VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_2_TRANSFER_BIT;
  case ERHIResourceUsage::RESOURCE_STATE_RENDER_TARGET:
    return VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
  case ERHIResourceUsage::RESOURCE_STATE_DEPTH_WRITE:
    return VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT;
  case ERHIResourceUsage::RESOURCE_STATE_DEPTH_READ:
    return VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT;
  case ERHIResourceUsage::RESOURCE_STATE_PRESENT:
    return VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT;
  case ERHIResourceUsage::RESOURCE_STATE_GENERIC_READ:
    return VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
  default:
    SK_LOG_ERROR("Unsupported Resource Usage");
    return 0;
  }
}
