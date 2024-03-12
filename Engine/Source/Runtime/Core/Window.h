#pragma once

#include <functional>
#include <memory>

#include "Types.h"
#include "StakMath.h"
#include "Platform.h"
#include "Event.h"

namespace sk {
/**
 * Config for window creation
 */
struct WindowConfig {
  i32 x;
  i32 y;
  i32 width;
  i32 height;
  const char *title;
};

class Window;

/**
 * Rendering Context
 */
class Context {
public:
  Context(const Window *win);
  ~Context();

  void makeCurrent();
  void swapBuffers();

private:
  const Window *window;
  PLATFORM_CONTEXT_STATE;
};

/**
 * Data for a window
 */
struct WindowData {
  i32 x;
  i32 y;
  i32 width;
  i32 height;
  const char *title;
  EventFn event_function;

  const Window *window;

  PLATFORM_WINDOW_STATE;
};

/**
 * Window class
 */
class Window {
public:
  Window(const WindowConfig &config);
  ~Window();

  void setPos(i32 x, i32 y);
  void setSize(i32 width, i32 height);
  Vec2 getPos();
  Vec2 getSize();

  void setEventCallback(const EventFn &func);

  void update();

  /**
   * Makes the this windows context current
   */
  void makeCurrent();

private:
  WindowData data;
  std::unique_ptr<Context> context;

  static u64 count;

  friend class Context;
};

} // namespace sk
