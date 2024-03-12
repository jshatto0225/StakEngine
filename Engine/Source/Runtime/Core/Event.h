#pragma once

#include "Types.h"

class Window;

/**
 * Events
 */
enum EventType {
  TEST_EVENT = -1,
  INVALID_EVENT = -1,
  WINDOW_RESIZED,
  WINDOW_MOVED,
  WINDOW_CLOSE,
  WINDOW_CREATE,
};

struct WindowCloseEvent {
  const Window *window;
};

struct WindowMoveEvent {
  const Window *window;
  i32 x = 0;
  i32 y = 0;
};

struct WindowResizeEvent {
  const Window *window;
  i32 width;
  i32 height;
};

struct Event {
  EventType type;
  WindowCloseEvent win_close_event;
  WindowMoveEvent win_move_event;
  WindowResizeEvent win_resize_event;
};
