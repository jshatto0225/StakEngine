#pragma once

#include "Types.h"
#include "RHICore.h"

// https://alextardif.com/RenderingAbstractionLayers.html

class IRHIGraphicsContext {
public:
  static TRef<IRHIGraphicsContext> Create(TRef<IRHIDevice> Device);

  virtual ~IRHIGraphicsContext() = default;

  virtual void Begin() = 0;
  virtual void End() = 0;

  virtual void SetRenderPass(TRef<IRHIRenderPass> RenderPass, FRHIRenderArea RenderArea) = 0;
  virtual void SetPipeline(TRef<IRHIPipeline> Pipeline) = 0;
  virtual void SetVertexBuffer(TRef<IRHIBuffer> Buffer) = 0;
  virtual void SetIndexBuffer(TRef<IRHIBuffer> Buffer) = 0;
  virtual void SetViewport(FSInt32 X, FSInt32 Y, FSInt32 Width, FSInt32 Height) = 0;
  virtual void Draw() = 0;
};

class IRHIComputeContext {
public:
  virtual ~IRHIComputeContext() = default;

  virtual void Begin() = 0;
  virtual void End() = 0;

  virtual void SetPipeline(TRef<IRHIPipeline> Pipeline) = 0;
  virtual void Dispatch() = 0;
};

class IRHIUploadContext {
public:
  virtual ~IRHIUploadContext() = default;

  virtual void Begin() = 0;
  virtual void End() = 0;

  virtual void UploadBuffer(TRef<IRHIBuffer> Buffer, void *Data) = 0;
  virtual void UploadTexture(TRef<IRHITexture> Texture, void *Data) = 0;
};
