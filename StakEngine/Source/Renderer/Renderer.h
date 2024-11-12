#pragma once

#include "Window.h"
#include "RHICore.h"
#include "IRHIBuffer.h"
#include "IRHIContext.h"
#include "IRHIDevice.h"
#include "IRHIPipeline.h"
#include "IRHIShader.h"
#include "IRHITexture.h"

#include <imgui.h>

namespace Stak {

class Renderer {
public:
  Renderer(Ref<Window> window);
  ~Renderer();

  void initImGui();
  void imGuiNewFrame();
  void shutdownImGui();

  Ref<IRHIBuffer> createBuffer(RHIBufferDescription &bufferDesc);
  Ref<IRHITexture> createTexture(RHITextureDescription &textureDesc);
  Ref<IRHIShader> createShader(RHIShaderDescription &shaderDesc);
  Ref<IRHIPipeline> createPipeline(RHIPipelineDescription &pipelineDesc);
  Ref<IRHIGraphicsContext> createGraphicsContext();

  void renderImGuiDrawData(ImDrawData *data, Ref<IRHIGraphicsContext> context);

  void processWindowChanges(Ref<Window> window);

private:
  Scope<IRHI> mRHI;
  Ref<IRHIDevice> mDevice;
};

} // namespace Stak
