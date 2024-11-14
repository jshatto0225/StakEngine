#pragma once

#include "Types.h"
#include "IWindow.h"

#include <imgui.h>

#define MAX_FRAMES_IN_FLIGHT 2

enum class ERHIFormat {
  R8_SINT,
  R8_UINT,
  R8_UNORM,
  R8_SNORM,
  R8_SRGB,
  R8G8_SINT,
  R8G8_UINT,
  R8G8_UNORM,
  R8G8_SNORM,
  R8G8_SRGB,
  R8G8B8_SINT,
  R8G8B8_UINT,
  R8G8B8_UNORM,
  R8G8B8_SNORM,
  R8G8B8_SRGB,
  R8G8B8A8_SINT,
  R8G8B8A8_UINT,
  R8G8B8A8_UNORM,
  R8G8B8A8_SNORM,
  R8G8B8A8_SRGB,
  
  R16_SINT,
  R16_UINT,
  R16_FLOAT,
  R16_UNORM,
  R16_SNORM,
  R16_SRGB,
  R16G16_SINT,
  R16G16_UINT,
  R16G16_FLOAT,
  R16G16_UNORM,
  R16G16_DNORM,
  R16G16_SRGB,
  R16G16B16_SINT,
  R16G16B16_UINT,
  R16G16B16_FLOAT,
  R16G16B16_UNORM,
  R16G16B16_SNORM,
  R16G16B16_SRGB,
  R16G16B16A16_SINT,
  R16G16B16A16_UINT,
  R16G16B16A16_FLOAT,
  R16G16B16A16_UNORM,
  R16G16B16A16_SNORM,
  R16G16B16A16_SRGB,
  
  R32_SINT,
  R32_UINT,
  R32_FLOAT,
  R32_UNORM,
  R32_SNORM,
  R32_SRGB,
  R32G32_SINT,
  R32G32_UINT,
  R32G32_FLOAT,
  R32G32_UNORM,
  R32G32_SNORM,
  R32G32_SRGB,
  R32G32B32_SINT,
  R32G32B32_UINT,
  R32G32B32_FLOAT,
  R32G32B32_UNORM,
  R32G32B32_SNORM,
  R32G32B32_SRGB,
  R32G32B32A32_SINT,
  R32G32B32A32_UINT,
  R32G32B32A32_FLOAT,
  R32G32B32A32_UNORM,
  R32G32B32A32_SNORM,
  R32G32B32A32_SRGB,
  
  R64_SINT,
  R64_UINT,
  R64_FLOAT,
  R64_UNORM,
  R64_SNORM,
  R64_SRGB,
  R64G64_SINT,
  R64G64_UINT,
  R64G64_FLOAT,
  R64G64_UNORM,
  R64G64_SNORM,
  R64G64_SRGB,
  R64G64B64_SINT,
  R64G64B64_UINT,
  R64G64B64_FLOAT,
  R64G64B64_UNORM,
  R64G64B64_SNORM,
  R64G64B64_SRGB,
  R64G64B64A64_SINT,
  R64G64B64A64_UINT,
  R64G64B64A64_FLOAT,
  R64G64B64A64_UNORM,
  R64G64B64A64_SNORM,
  R64G64B64A64_SRGB,

  D32_FLOAT,
  D24_S8_UINT,
  S8_UINT,
};

enum class ERHIShaderStage {
  NONE,
  VERTEX,
  FRAGMENT,
};

enum class ERHIBufferUsage {
  STAGING,
  VERTEX_DEVICE,
  VERTEX_SHARED,
  INDEX_DEVICE,
  INDEX_SHARED,
  UNIFORM_DEVICE,
  UNIFORM_SHARED,
};

enum class ERHIPipelineBindPoint {
  GRAPHICS,
  COMPUTE,
  RAY_TRACING
};

enum class ERHIImageState {
  UNDEFINED,
  COLOR_ATTACHMENT,
  DEPTH_STENCIL,
  SHADER_READ,
  PRESENT,
  TRANSFER_SOURCE,
  TRANSFER_DESTINATION,
  DEPTH_READ,
};

enum class ERHIPipelineStage {
  TOP_OF_PIPE,
  VERTEX_INPUT,
  VERTEX_SHADER,
  TESSELATION_CONTROL_SHADER,
  TESSELATION_EVALUATION_SHADER,
  GEOMETRY_SHADER,
  RASTERIZER,
  FRAGMENT_SHADER,
  FRAMEBUFFER_WRITE,
  COMPUTE_SHADER,
  BOTTOM_OF_PIPE,
};

enum class ERHIAccess {
  INDEX_READ,
  VERTEX_ATTRIBUTE_READ,
  UNIFORM_READ,
  INPUT_ATTACHMENT_READ,
  SHADER_READ,
  SHADER_WRITE,
  COLOR_ATTACHMENT_READ,
  COLOR_ATTACHMENT_WRITE,
  DEPTH_STENCIL_ATTACHMENT_READ,
  DEPTH_STENCIL_ATTACHMENT_WRITE,
  TRANSFER_READ,
  TRANSFER_WRITE,
  HOST_READ,
  HOST_WRITE,
  MEMORY_READ,
  MEMORY_WRITE,
};

enum class ERHILoadOp {
  LOAD,
  CLEAR,
  DISCARD,
};

enum class ERHIStoreOp {
  DISCARD,
  STORE,
};

struct FRHIShaderDescription {

};

struct FRHIResourceBarrierDescription {

};

struct FRHITextureDescription {
  bool IsSwapchainImage;
  FUInt32 SwapchainImageIndex;
};

struct FRHIRenderPassAttachmentDescription {
  ERHIImageState InitialState;
  ERHIImageState FinalState;
  ERHIFormat Format;
  ERHILoadOp LoadOp;
  ERHILoadOp StencilLoadOp;
  ERHIStoreOp StoreOp;
  ERHIStoreOp StencilStoreOp;
};

struct FRHISubpassDescription {
  std::vector<FRHIRenderPassAttachmentDescription> InputAttachments;
  std::vector<FRHIRenderPassAttachmentDescription> ColorAttachments;
  std::vector<FRHIRenderPassAttachmentDescription> ResolveAttachments;
  std::vector<FRHIRenderPassAttachmentDescription> PreserveAttachments;
  FRHIRenderPassAttachmentDescription DepthStencilAttachment;
};

struct SubpassDependency {
  FUInt32 SourceSubpass;
  FUInt32 DestinationSubpass;
  ERHIPipelineStage SourceStage;
  ERHIAccess SourceAccess;
  ERHIPipelineStage DestinationStage;
  ERHIAccess DestinationAccess;
};

struct FRHIRenderPassDescription {
  std::vector<FRHIRenderPassAttachmentDescription> Attachments;
  std::vector<FRHISubpassDescription> Subpasses;
  std::vector<SubpassDependency> Dependencies;
};

struct FRHIPipelineDescription {

};

struct FRHIBufferElement {
  ERHIFormat Format;
  FUInt32 ByteOffset;
};

struct FRHIBufferDescription {
  ERHIBufferUsage Usage;
  std::vector<FRHIBufferElement> Elements;
  FUInt32 Size;
  ERHIShaderStage ShaderAccess;
};

class IRHIContext;
class IRHIGraphicsContext;
class IRHIComputeContext;
class IRHIUploadContext;
class IRHIDevice;
class IRHIInstance;
class IRHIPipeline;
class IRHIRenderPass;
class IRHIResource;
class IRHIBuffer;
class IRHITexture;
class IRHIShader;
class IRHISyncObject;

FUInt32 GetSizeOfRHIFormat(ERHIFormat Format);
