#pragma once

#include <functional>

#include "Event.h"
#include "Types.h"

namespace Stak {

struct WindowConfig {
  i32 X;
  i32 Y;
  i32 Width;
  i32 Height;
  const char *Title;
};

struct WindowSizeData {
  i32 Width;
  i32 Height;
};

struct WindowPosData {
  i32 X;
  i32 Y;
};

class Window {
  using EventFn = std::function<void(Event &)>;
public:
  virtual void Update() = 0;

  virtual WindowSizeData GetWindowSize() = 0;
  virtual WindowPosData GetWindowPos() = 0;

  virtual void SetEventFn(const EventFn &func) = 0;

  static Scope<Window> Create(const WindowConfig &cfg);
};

} // namespace Stak
