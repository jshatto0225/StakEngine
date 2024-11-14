#pragma once

#include <functional>

#include "Events.h"
#include "Types.h"
#include "IInput.h"

struct FWindowConfig {
  FSInt32 Width;
  FSInt32 Height;
  const char *Title;
};

struct FWindowSizeData {
  FSInt32 Width;
  FSInt32 Height;
};

struct FWindowPosData {
  FSInt32 X;
  FSInt32 Y;
};

class IWindow {
public:

  IWindow() = default;
  virtual ~IWindow() {}

  virtual void Update() = 0;

  virtual FWindowSizeData GetSize() = 0;
  virtual FWindowPosData GetPos() = 0;

  using FWindowResizeEventFn = std::function<void(FWindowResizeEvent &)>;
  virtual void SetResizeEventFn(const FWindowResizeEventFn &Func) = 0;

  using FWindowCloseEventFn = std::function<void()>;
  virtual void SetCloseEventFn(const FWindowCloseEventFn &Func) = 0;

  virtual void InitImGui() = 0;
  virtual void ImGuiNewFrame() = 0;
  virtual void ShutdownImGui() = 0;

  virtual IInput::FMousePosData GetMousePos() = 0;
  virtual bool KeyDown(EKeyCode Key) = 0;
  virtual bool MouseButtonDown(EMouseCode Button) = 0;

  static TScope<IWindow> Create(const FWindowConfig &Cfg);
};
