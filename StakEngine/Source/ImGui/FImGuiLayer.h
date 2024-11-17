#pragma once

#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>

#include "IApplicationLayer.h"
#include "IWindow.h"
#include "IRHIDevice.h"
#include "IRHIContext.h"
#include "IRHIRenderPass.h"
#include "IRHIFramebuffer.h"
#include "IRHIResource.h"
#include "IRHISyncObject.h"

class FImGuiLayer final : public IApplicationLayer {
public:
  FImGuiLayer(TRef<IRHIDevice> Device, TRef<IWindow> Window);
  ~FImGuiLayer();

  void BeginFrame();
  void EndFrame();
  void OnWindowResize(const FWindowResizeEvent &Event);

private:
  TRef<IRHIGraphicsContext> mGraphicsContext;
  TRef<IRHIRenderPass> mRenderPass;
  std::vector<TRef<IRHIFramebuffer>> mSwapchainFramebuffers;
  TRef<IRHIDevice> mDevice;
  TRef<IWindow> mWindow;
  FUInt32 mCurrentFrame;
  TRef<IRHISyncObject> mImageAvailableSemaphore;
};
