#pragma once

#include "Window.h"
#include "RHICore.h"
#include "IRHIBuffer.h"
#include "IRHIContext.h"
#include "IRHIDevice.h"
#include "IRHIPipeline.h"
#include "IRHIShader.h"
#include "IRHITexture.h"

namespace Stak {
class Renderer {
public:
  Renderer(Ref<Window> window);
  ~Renderer();

  void initImGui();
  void imGuiNewFrame();

  void processWindowChanges(Ref<Window> window);

private:
  Scope<IRHI> mRHI;
  Ref<IRHIDevice> mDevice;
};
} // namespace Stak
