#pragma once

#include <functional>

#include "Event.h"
#include "Types.h"

namespace Stak {

struct WindowConfig {
  i32 width;
  i32 height;
  const char *title;
};

struct WindowSizeData {
  i32 width;
  i32 height;
};

struct WindowPosData {
  i32 x;
  i32 y;
};

class Window {
public:
  using EventFn = std::function<void(Event &)>;

  Window() = default;
  virtual ~Window() {}

  virtual void update() = 0;

  virtual WindowSizeData getWindowSize() = 0;
  virtual WindowPosData getWindowPos() = 0;

  virtual void setEventFn(const EventFn &func) = 0;

  virtual void *getNativeHandle() const = 0;

  static Scope<Window> create(const WindowConfig &cfg);
};

} // namespace Stak
