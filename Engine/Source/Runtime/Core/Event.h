#pragma once

#include "Types.h"

struct SKWindow;

enum SKEventType {
  SK_TEST_EVENT = -1,
  SK_INVALID_EVENT = -1,
  SK_WINDOW_RESIZED,
  SK_WINDOW_MOVED,
  SK_WINDOW_CLOSE,
};

struct SKWindowCloseEvent {
  SKWindow *window = nullptr;
};

struct SKWindowMoveEvent {
  SKWindow *window = nullptr;
  i32 x = 0;
  i32 y = 0;
};

struct SKWindowResizeEvent {
  SKWindow *window = nullptr;
  i32 width = 0;
  i32 height = 0;
};

struct SKEvent {
  SKEventType type = SK_INVALID_EVENT;
  SKWindowCloseEvent win_close_event;
  SKWindowMoveEvent win_move_event;
  SKWindowResizeEvent win_resize_event;
};

