#pragma once

#include "Types.h"

struct window;

/**
 * Events
 */
enum event_type {
  TEST_EVENT = -1,
  INVALID_EVENT = -1,
  WINDOW_RESIZED,
  WINDOW_MOVED,
  WINDOW_CLOSE,
  WINDOW_CREATE,
  WINDOW_PAINT,
};

struct window_paint_event {
  window *Window;
};

struct window_close_event {
  const window *Window;
};

struct window_move_event {
  const window *Window;
  i32 X;
  i32 Y;
};

struct window_resize_event {
  const window *Window;
  i32 Width;
  i32 Height;
};

struct event {
  event_type Type;
  window_close_event WinCloseEvent;
  window_move_event WinMoveEvent;
  window_resize_event WinResizeEvent;
  window_paint_event WinPaintEvent;
};
