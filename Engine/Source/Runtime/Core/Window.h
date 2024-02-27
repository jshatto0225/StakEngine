#pragma once

#include <functional>

#include "Types.h"
#include "Platform.h"
#include "Event.h"

namespace sk {
struct WindowConfig {
  i32 x;
  i32 y;
  i32 width;
  i32 height;
  const char *title;
};

class Window;

class Context {
public:

  Context(Window *win);
  ~Context();

  void make_current();
  void swap_buffers();

private:
  Window *window;
  PLATFORM_CONTEXT_STATE;
};

struct WindowData {
  i32 x;
  i32 y;
  i32 width;
  i32 height;
  const char *title;
  bool should_close;
  std::function<void(Event &)> event_function;

  const Window *window;

  PLATFORM_WINDOW_STATE;
};

class Window {
public:
  Window(const WindowConfig &config);
  ~Window();

  void set_pos(i32 x, i32 y);
  void set_size(i32 width, i32 height);
  void get_pos(i32 *x, i32 *y);
  void get_size(i32 *width, i32 *height);

  void set_event_callback(const std::function<void(Event &)> &func);

  void update();

  void make_current();

private:
  WindowData data;
  Context *context;

  static u64 count;

  friend class Context;
};
}