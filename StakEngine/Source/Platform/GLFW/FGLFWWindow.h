#pragma once

#include <GLFW/glfw3.h>

#include <string>

#include "IWindow.h"
#include "Events.h"

class FGLFWWindow final : public IWindow {
public:
  FGLFWWindow(const FWindowConfig &Cfg);
  ~FGLFWWindow() override;

  void Update() override;

  FWindowSizeData GetSize() override;
  FWindowPosData GetPos() override;

  void InitImGui() override;
  void ImGuiNewFrame() override;
  void ShutdownImGui() override;

  void SetResizeEventFn(const FWindowResizeEventFn &Func) override;
  void SetCloseEventFn(const FWindowCloseEventFn &Func) override;


  IInput::FMousePosData GetMousePos() override;
  bool KeyDown(EKeyCode Key) override;
  bool MouseButtonDown(EMouseCode Button) override;

public:
  inline GLFWwindow *GetGlfwWindow() { return mNativeHandle; }

private:
  struct FWindowData {
    FWindowCloseEventFn WindowCloseEventFn;
    FWindowResizeEventFn WindowResizeEventFn;
    FSInt32 X;
    FSInt32 Y;
    FSInt32 Width;
    FSInt32 Height;
    std::string Title;
  };

  FWindowData mData;
  GLFWwindow *mNativeHandle = NULL;

  static FBool sGLFWInitialized;
};
