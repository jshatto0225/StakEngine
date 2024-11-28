#pragma once

#include <vector>
#include <string>

#include "IApplicationLayer.h"
#include "Events.h"
#include "FRenderer.h"
#include "FInput.h"
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

  static FApplication &Get() { ASSERT(sInstance);  return *sInstance; }

  void Run();
  void AddLayer(IApplicationLayer *layer);
  void Close();
  
  inline const char *GetName() const { return mName.c_str(); }
  inline const TRef<IWindow> GetWindow() const { return mWindow; }

private:
  void OnWindowClose();
  void OnWindowResize(const FWindowResizeEvent &Event);
  void OnKeyEvent(const FKeyEvent &Event);
  void OnMouseButtonEvent(const FMouseButtonEvent &Event);
  void OnMouseMoveEvent(const FMouseMoveEvent &Event);

private:
  inline static FApplication *sInstance = NULL;

private:
  TRef<IWindow> mWindow;
  TRef<FRenderer> mRenderer;
  TRef<FInput> mInput;
  FLayerStack mLayerStack;
  FBool mRunning;
  FImGuiLayer *mImGuiLayer;
  std::string mName = "Stak Application";
};
