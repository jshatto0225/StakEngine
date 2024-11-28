#pragma once

#include "Types.h"

class IRHIViewableResource;
using FRHIViewableResourceRef = TRef<IRHIViewableResource>;
class IRHIPixelShader;
using FRHIPixelShaderRef = TRef<IRHIPixelShader>;
class IRHIVertexShader;
using FRHIVertexShaderRef = TRef<IRHIVertexShader>;
class IRHIGeometryShader;
using FRHIGeometryShaderRef = TRef<IRHIGeometryShader>;
class IRHIBuffer;
using FRHIBufferRef = TRef<IRHIBuffer>;
class IRHIUniformBuffer;
using FRHIUniformBufferRef = TRef<IRHIUniformBuffer>;
class IRHIStagingBuffer;
using FRHIStagingBufferRef = TRef<IRHIStagingBuffer>;
class IRHITexture;
using FRHITexture = TRef<IRHITexture>;
class IRHIGraphicsPipelineState;
using FRHIGraphicsPipelineStateRef = TRef<IRHIGraphicsPipelineState>;
class IRHISamplerState;
using FRHISamplerStateRef = TRef<IRHISamplerState>;
class IRHIRasterizerState;
using FRHIRasterizerStateRef = TRef<IRHIRasterizerState>;
class IRHIDepthStencilState;
using FRHIDepthStencilStateRef = TRef<IRHIDepthStencilState>;
class IRHIBlendState;
using FRHIBlendStateRef = TRef<IRHIBlendState>;
class IRHIVertexDeclaration;
using FRHIVertexDeclarationRef = TRef<IRHIVertexDeclaration>;
class IRHIShaderResourceView;
using FRHIShaderResourceViewRef = TRef<IRHIShaderResourceView>;
class IRHIUnorderedAccessView;
using FRHIUnorderedAccessViewRef = TRef<IRHIUnorderedAccessView>;
class FRHICommandList;
class IRHICommandContext;
class IRHIViewport;
using FRHIViewportRef = TRef<IRHIViewport>;
class IRHITexture;
using FRHITextureRef = TRef<IRHITexture>;
class IRHITransition;