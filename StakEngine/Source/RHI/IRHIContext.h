#pragma once

#include "Types.h"
#include "RHICore.h"

// https://alextardif.com/RenderingAbstractionLayers.html

class IRHIContext {
public:
  virtual ~IRHIContext() = default;

  virtual ERHIContextType GetType() const = 0;
};

class IRHIGraphicsContext : virtual public IRHIContext {
public:
  static TRef<IRHIGraphicsContext> Create(TRef<IRHIDevice> Device);

  virtual ~IRHIGraphicsContext() = default;

  virtual void Begin() = 0;
  virtual void End() = 0;

  virtual void SetPipeline(TRef<IRHIPipeline> Pipeline) = 0;
  virtual void SetIndexBuffer(TRef<IRHIBuffer> Buffer) = 0;
  virtual void SetVertexBuffers(std::vector<TRef<IRHIBuffer>> Buffers) = 0;
  virtual void SetStreamOutputTargets(std::vector<TRef<IRHITexture>> Textures) = 0;
  virtual void SetRenderTargets(std::vector<TRef<IRHITexture>> Textures, const FRHIRect &RenderArea) = 0;
  virtual void UnsetRenderTargets() = 0;
  virtual void SetDescriptorSet(TRef<IRHIDescritporSet> Set) = 0;
  virtual void SetViewports(const std::vector<FRHIViewport> &Viewports) = 0;
  virtual void SetScissors(const std::vector<FRHIRect> &Scissors) = 0;
  virtual void SetBlendConstants(std::array<FFloat, 4> Constants) = 0;
  virtual void SetDepthStencilReferenceValue(FUInt32 Val) = 0;
  virtual void SetTopology(ERHITopology Topology) = 0;

  virtual void ResourceBarrier(const FRHIResourceBarrierDescription &Description) = 0;
  
  virtual void DrawIndexed(FUInt32 FirstIndex, FUInt32 IndexCount, FUInt32 FirstInstance, FUInt32 InstanceCount, FSInt32 VertexOffset) = 0;
  virtual void DrawInstanced(FUInt32 FirstVertex, FUInt32 VertexCount, FUInt32 FirstInstance, FUInt32 InstanceCount) = 0;
};

class IRHIComputeContext : virtual public IRHIContext {
public:
  virtual ~IRHIComputeContext() = default;

  virtual void Begin() = 0;
  virtual void End() = 0;

  virtual void SetPipeline(TRef<IRHIPipeline> Pipeline) = 0;
  virtual void Dispatch() = 0;
};

class IRHIUploadContext : virtual public IRHIContext {
public:
  virtual ~IRHIUploadContext() = default;

  virtual void Begin() = 0;
  virtual void End() = 0;

  virtual void UploadBuffer(TRef<IRHIBuffer> Buffer, void *Data) = 0;
  virtual void UploadTexture(TRef<IRHITexture> Texture, void *Data) = 0;
};
