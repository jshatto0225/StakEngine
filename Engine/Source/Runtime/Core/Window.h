#pragma once

#include <functional>
#include <memory>

#include "Types.h"
#include "StakMath.h"
#include "Platform.h"
#include "Event.h"

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

  void makeCurrent();
  void swapBuffers();

private:
  const Window *window;
  PLATFORM_CONTEXT_STATE;
};

struct WindowData {
  i32 x;
  i32 y;
  i32 width;
  i32 height;
  const char *title;
  std::function<void(Event &)> event_function;

  const Window *window;

  PLATFORM_WINDOW_STATE;
};

class Window {
public:
  Window(const WindowConfig &config);
  ~Window();

  void setPos(i32 x, i32 y);
  void setSize(i32 width, i32 height);
  Vec2 getPos();
  Vec2 getSize();

  void setEventCallback(const std::function<void(Event &)> &func);

  void update();

  void makeCurrent();

private:
  WindowData data;
  std::unique_ptr<Context> context;

  static u64 count;

  friend class Context;
};
