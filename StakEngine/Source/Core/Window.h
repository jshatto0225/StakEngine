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
  using EventFn = std::function<void(IEvent &)>;

  Window() = default;
  virtual ~Window() {}

  virtual void update() = 0;

  virtual WindowSizeData getSize() = 0;
  virtual WindowPosData getPos() = 0;

  virtual void setEventFn(const EventFn &func) = 0;

  virtual OpaqueHandle getNativeHandle() const = 0;

  virtual void initImGui() = 0;
  virtual void imGuiNewFrame() = 0;

  virtual void shutdownImGui() = 0;

  static Scope<Window> create(const WindowConfig &cfg);
};

} // namespace Stak
