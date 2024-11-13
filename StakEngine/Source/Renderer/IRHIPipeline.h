#pragma once

#include "IRHIBuffer.h"
#include "IRHIShader.h"
#include "IRHITexture.h"

#include <string>

namespace Stak {

enum class ERHICullMode {
  NONE,
  FRONT,
  BACK
};

enum class ERHIFillMode {
  SOLID,
  WIREFRAME
};

enum class ERHISampleCount {
  ONE,
  TWO,
  FOUR,
  EIGHT,
  SIXTEEN
};

enum class ERHIBlendFactor {
  ZERO,
  ONE,
  SRC_COLOR,
  ONE_MINUS_SRC_COLOR,
  DST_COLOR,
  ONE_MINUS_DST_COLOR,
  SRC_ALPHA,
  ONE_MINUS_SRC_ALPHA,
  DST_ALPHA,
  ONE_MINUS_DST_ALPHA,
  CONSTANT_COLOR,
  ONE_MINUS_CONSTANT_COLOR,
  CONSTANT_ALPHA,
  ONE_MINUS_CONSTANT_ALPHA,
  SRC_ALPHA_SATURATE
};

enum class ERHIBlendOperation {
  ADD,
  SUBTRACT,
  REVERSE_SUBTRACT,
  MIN,
  MAX
};

struct RHIRasterizationState {
  ERHICullMode cullMode;
  ERHIFillMode fillMode;
  bool depthBiasEnable;
  f32 depthBias;
  f32 slopeScaledDepthBias;
  float lineWidth;
  bool frontFaceClockwise;
};

struct RHIMultisamplingState {
  ERHISampleCount sampleCount;
  bool sampleShadingEnable;
  float minSampleShading;
  bool alphaToCoverageEnable;
  bool alphaToOneEnable;
};

struct RHIColorBlendAttachmentState {
  bool blendEnable;
  ERHIBlendFactor srcBlendFactor;
  ERHIBlendFactor dstBlendFactor;
  ERHIBlendOperation blendOp;
  ERHIBlendFactor srcBlendAlpha;
  ERHIBlendFactor dstBlendAlpha;
  ERHIBlendOperation blendOpAlpha;
  uint32_t colorMask;
};

struct RHIColorBlendState {
  std::vector<RHIColorBlendAttachmentState> attachments;
};

enum class ERHITopology {
  POINT,
  LINE,
  TRIANGLE,
  PATCH
};

enum class ERHIComparisonFunc {
  NEVER,
  LETT,
  EQUAL,
  LESS_EQUAL,
  GREATER,
  NOT_EQUAL,
  GREATER_EQUAL,
  ALWAYS
};

enum class ERHIStencilOp {
  KEEP,
  ZERO,
  REPLACE,
  INC_AND_CLAMP,
  DEC_AND_CLAMP,
  INVERT,
  INC_AND_WRAP,
  DEC_AND_WRAP
};

struct RHIDepthStencilState {
  bool depthTestEnable;
  bool depthWriteEnable;
  ERHIComparisonFunc depthCompareFunc;
  bool stencilTestEnable;
  u32 stencilReadMask;
  u32 stencilWriteMask;
  ERHIStencilOp frontFaceStencilFailOp;
  ERHIStencilOp frontFaceStencilDepthFailOp;
  ERHIStencilOp frontFaceStencilPassOp;
  ERHIComparisonFunc frontFaceStencilCompareFunc;
  ERHIStencilOp backFaceStencilFailOp;
  ERHIStencilOp backFaceStencilDepthFailOp;
  ERHIStencilOp backFaceStencilPassOp;
  ERHIComparisonFunc backFaceStencilCompareFunc;
};

struct RHIPipelineDescription {
  Ref<IRHIBuffer> vertexInput;
  Ref<IRHIShader> vertexShader;
  Ref<IRHIShader> fragmentShader;
  Ref<IRHIShader> geometryShader;
  Ref<IRHIShader> tessControlShader;
  Ref<IRHIShader> tessEvaluationShader;
  Ref<IRHIShader> computeShader;
  RHIRasterizationState rasterizer;
  RHIMultisamplingState multisampling;
  RHIColorBlendState blending;
  RHIDepthStencilState depthStencil;
  ERHITopology topology;

  std::vector<IRHITexture> textures;
  std::vector<IRHIBuffer> buffers;

  std::string debugName;
};

class IRHIPipeline {
public:
  virtual ~IRHIPipeline() = default;

  virtual void bind() = 0;
};

} // namespace Stak