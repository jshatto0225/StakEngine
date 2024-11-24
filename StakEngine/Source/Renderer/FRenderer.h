#pragma once

#include "RHICore.h"

#include <imgui.h>

class FRenderer {
public:
  FRenderer(TRef<IWindow> window);
  ~FRenderer();

  void InitImGui();
  void ImGuiNewFrame();
  void SubmitImGuiDrawData(ImDrawData *DrawData);
  void ShutdownImGui();
  void Render();

private:
  TRef<IRHIInstance> mRHIInstance;
  TRef<IRHIDevice> mRHIDevice;
  TRef<IWindow> mWindow;

private:
  TRef<IRHIGraphicsContext> mImGuiRHIGraphicsContext;
  TRef<IRHITexture> mImGuiRHISwapchainTexture;
};
