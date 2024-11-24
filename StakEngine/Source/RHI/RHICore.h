#pragma once

#include "Types.h"
#include "IWindow.h"

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
class IRHIResource;
class IRHIBuffer;
class IRHITexture;
class IRHIShader;
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

enum class ERHIResourceUsage {
  RESOURCE_STATE_UNDEFINED,

  RESOURCE_STATE_VERTEX_BUFFER,
  RESOURCE_STATE_CONSTANT_BUFFER,
  RESOURCE_STATE_INDEX_BUFFER,
  RESOURCE_STATE_INDIRECT_ARGUMENT,

  RESOURCE_STATE_COPY_SOURCE,
  RESOURCE_STATE_COPY_DEST,

  RESOURCE_STATE_SHADER_RESOURCE,
  RESOURCE_STATE_UNORDERED_ACCESS,

  RESOURCE_STATE_RENDER_TARGET,
  RESOURCE_STATE_DEPTH_WRITE,
  RESOURCE_STATE_DEPTH_READ,

  RESOURCE_STATE_PRESENT,
  RESOURCE_STATE_GENERIC_READ
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
  POINT,
  LINE,
  TRIANGLE,
  PATCH,
};

enum class ERHIContextType {
  GRAPHICS
};

enum class ERHIResourceType {
  BUFFER,
  TEXTURE
};

struct FRHIShaderDescription {

};

struct FRHITransitionBarrierDescription {
  TRef<IRHIResource> Resource;
  ERHIResourceUsage NewUsage;
};

struct FRHIAliasingBarrierDescription {
  TRef<IRHIResource> Before;
  TRef<IRHIResource> After;
};

struct FRHIUAVBarrierDescription {
  TRef<IRHIResource> Resource;
};

struct FRHIResourceBarrierDescription {
  std::vector<FRHITransitionBarrierDescription> Transitions;
};

struct FRHIClearValue {
  FFloat Color[4];
  struct {
    FFloat Depth;
    FUInt8 Stencil;
  } DepthStencil;
};

struct FRHITextureDescription {
  bool IsSwapchainImage;

  FUInt32 Width;
  FUInt32 Height;
  FUInt32 Layers;

  ERHIStoreOp StoreOp;
  ERHILoadOp LoadOp;

  ERHIResourceUsage Usage;

  FRHIClearValue ClearValue;
};

struct FRHIPipelineDescription {

};

struct FRHIViewport {
  FFloat X;
  FFloat Y;
  FFloat Width;
  FFloat Height;
  FFloat MinDepth;
  FFloat MaxDepth;
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
