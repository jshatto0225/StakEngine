#pragma once

#include "Types.h"
#include "Window.h"
#include "IRHIBuffer.h"
#include "IRHIPipeline.h"
#include "IRHITexture.h"
#include "IRHIShader.h"
#include "IRHIContext.h"

namespace Stak {

class IRHIDevice {
public:
  virtual ~IRHIDevice() = default;

  virtual void processWindowChanges(Ref<Window> window) = 0;
  virtual Ref<IRHIBuffer> createBuffer(BufferDescription &bufferDesc) = 0;
  virtual Ref<IRHITexture> createTexture(TextureDescription &textureDesc) = 0;
  virtual Ref<IRHIShader> createShader(ShaderDescription &shaderDesc) = 0;
  virtual Ref<IRHIPipeline> createPipeline(PipelineDescription &pipelineDesc) = 0;
  virtual Ref<IRHIGraphicsContext> createGraphicsContext() = 0;

  virtual Scope<IRHIRecipt> submitWork(Ref<IRHIContext> context) = 0;
  virtual void waitOnWork(Scope<IRHIRecipt> recipt) = 0;

  virtual void present() = 0;
};

} // namespace Stak