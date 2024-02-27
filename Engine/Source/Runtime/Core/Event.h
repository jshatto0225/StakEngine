#pragma once

#include "Types.h"

namespace sk {

class Window;

enum EventType {
  TEST_EVENT = -1,
  INVALID_EVENT = -1,
  WINDOW_RESIZED,
  WINDOW_MOVED,
  WINDOW_CLOSE,
};

struct WindowCloseEvent {
  const Window *window = nullptr;
};

struct WindowMoveEvent {
  const Window *window = nullptr;
  i32 x = 0;
  i32 y = 0;
};

struct WindowResizeEvent {
  const Window *window = nullptr;
  i32 width = 0;
  i32 height = 0;
};

struct Event {
  EventType type = INVALID_EVENT;
  WindowCloseEvent win_close_event;
  WindowMoveEvent win_move_event;
  WindowResizeEvent win_resize_event;
};
}
