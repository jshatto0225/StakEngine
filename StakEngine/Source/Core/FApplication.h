#pragma once

#include <vector>

#include "IApplicationLayer.h"
#include "Events.h"
#include "Renderer.h"
#include "IInput.h"
#include "FImGuiLayer.h"

struct FApplicationSpec {
  const char *WindowTitle;
  FSInt32 WindowWidth;
  FSInt32 WindowHeight;
};

class FApplication final {
public:
  FApplication(const FApplicationSpec &Spec);
  ~FApplication();

  void Run();
  void OnWindowClose();
  void OnWindowResize(FWindowResizeEvent &event);
  void AddLayer(IApplicationLayer *layer);
  void Close();

  inline TRef<const IInput> GetInput() const { return mInput; }

private:
  TRef<IWindow> mWindow;
  TRef<Renderer> mRenderer;
  FLayerStack mLayerStack;
  FBool mRunning;
  FImGuiLayer *mImGuiLayer;
  TRef<const IInput> mInput;

private:
  TRef<IRHIInstance> mRHIInstance;
  TRef<IRHIDevice> mRHIDevice;
};
