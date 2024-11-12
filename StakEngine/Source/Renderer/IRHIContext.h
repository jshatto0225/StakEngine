#pragma once

#include "Types.h"
#include "IRHIBuffer.h"
#include "IRHIPipeline.h"
#include "IRHITexture.h"

namespace Stak {

// https://alextardif.com/RenderingAbstractionLayers.html

struct ResourceBarrierDescription {

};

enum class ERHIContextType {
  GRAPHICS,
  COMPUTE,
  UPLOAD
};

class IRHIContext {
public:
  virtual ~IRHIContext() = default;

  virtual void begin() = 0;
  virtual void end() = 0;
  virtual void resourceBarrier(ResourceBarrierDescription &barrierDesc) = 0;
  virtual ERHIContextType getType() = 0;
};

class IRHIGraphicsContext : public IRHIContext {
public:
  virtual ~IRHIGraphicsContext() = default;

  virtual void setPipeline(Ref<IRHIPipeline> pipeline) = 0;
  virtual void setVertexBuffer(Ref<IRHIBuffer> buffer) = 0;
  virtual void setIndexBuffer(Ref<IRHIBuffer> buffer) = 0;
  virtual void draw() = 0;
};

class IRHIComputeContext : public IRHIContext {
public:
  virtual ~IRHIComputeContext() = default;

  virtual void setPipeline(Ref<IRHIPipeline> pipeline) = 0;
  virtual void dispatch() = 0;
};

class IRHIUploadContext : public IRHIContext {
public:
  virtual ~IRHIUploadContext() = default;

  virtual void uploadBuffer(Ref<IRHIBuffer> buffer, void *data) = 0;
  virtual void uploadTexture(Ref<IRHITexture> texture, void *data) = 0;
};

} // namespace Stak