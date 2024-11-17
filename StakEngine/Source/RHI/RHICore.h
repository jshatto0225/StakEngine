#pragma once

#include "Types.h"
#include "IWindow.h"

#include <imgui.h>
#include <map>
#include <optional>

#define MAX_FRAMES_IN_FLIGHT 2
#define EXTERNAL_SUBPASS (~0U)

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
class IRHIFramebuffer;
class IRHIDescritporSet;
class IRHIWorkRecipt;

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

  B8G8R8A8_UNORM,
  B8G8R8A8_UNORM_SRGB,
  
  R16_SINT,
  R16_UINT,
  R16_FLOAT,
  R16_UNORM,
  R16_SNORM,
  R16G16_SINT,
  R16G16_UINT,
  R16G16_FLOAT,
  R16G16_UNORM,
  R16G16_SNORM,
  R16G16B16_SINT,
  R16G16B16_UINT,
  R16G16B16_FLOAT,
  R16G16B16_UNORM,
  R16G16B16_SNORM,
  R16G16B16A16_SINT,
  R16G16B16A16_UINT,
  R16G16B16A16_FLOAT,
  R16G16B16A16_UNORM,
  R16G16B16A16_SNORM,
  
  R32_SINT,
  R32_UINT,
  R32_FLOAT,
  R32G32_SINT,
  R32G32_UINT,
  R32G32_FLOAT,
  R32G32B32_SINT,
  R32G32B32_UINT,
  R32G32B32_FLOAT,
  R32G32B32A32_SINT,
  R32G32B32A32_UINT,
  R32G32B32A32_FLOAT,
  
  R64_SINT,
  R64_UINT,
  R64_FLOAT,
  R64G64_SINT,
  R64G64_UINT,
  R64G64_FLOAT,
  R64G64B64_SINT,
  R64G64B64_UINT,
  R64G64B64_FLOAT,
  R64G64B64A64_SINT,
  R64G64B64A64_UINT,
  R64G64B64A64_FLOAT,

  D32_FLOAT,
  D24_UNORM_S8_UINT,
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
};

enum class ERHIImageUsage {
  UNDEFINED,
  COLOR_ATTACHMENT,
  DEPTH_STENCIL,
  SHADER_READ,
  PRESENT,
  TRANSFER_SOURCE,
  TRANSFER_DESTINATION,
};

enum class ERHIPipelineStage {
  TOP_OF_PIPE,
  VERTEX_INPUT,
  VERTEX_SHADER,
  TESSELLATION_CONTROL_SHADER,
  TESSELLATION_EVALUATION_SHADER,
  GEOMETRY_SHADER,
  FRAGMENT_SHADER,
  FRAMEBUFFER_WRITE,
  COMPUTE_SHADER,
  BOTTOM_OF_PIPE,
};

enum class ERHIAccess {
  DONT_CARE,
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
  DISCARD,
  LOAD,
  CLEAR,
};

enum class ERHIStoreOp {
  DISCARD,
  STORE,
};

enum class ERHISampleCount {
  ONE,
  TWO,
  FOUR,
  EIGHT,
  SIXTEEN,
  THIRTY_TWO,
  SIXTY_FOUR
};

enum class ERHITopology {

};

struct FRHIShaderDescription {

};

struct FRHIResourceBarrierDescription {

};

struct FRHITextureDescription {
  bool IsSwapchainImage;

  FUInt32 Width;
  FUInt32 Height;
  FUInt32 Layers;

  ERHIStoreOp StoreOp;
  ERHILoadOp LoadOp;

  ERHIImageUsage Usage;
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

struct FRHIRect {
  FUInt32 X;
  FUInt32 Y;
  FUInt32 Width;
  FUInt32 Height;
};

class IRHIWorkRecipt {
public:
  virtual ~IRHIWorkRecipt() = default;
};

FUInt32 GetSizeOfRHIFormat(ERHIFormat Format);
