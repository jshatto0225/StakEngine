#pragma once

#include "Types.h"

namespace sk {
enum EventType {
  TEST_EVENT = -1,
  INVALID_EVENT = -1,
  WINDOW_RECT_CHANGED,
  WINDOW_CLOSE,
};

struct Event {
  EventType type;
};

struct WindowCloseEvent : public Event {
  WindowCloseEvent(u64 window_id) {
    this->type = WINDOW_CLOSE;
    this->window_id = window_id;
  }

  u64 window_id;
};

struct WindowRectChangedEvent : public Event {
  WindowRectChangedEvent(u64 window_id, i32 width, i32 height, i32 x, i32 y) {
    this->type = WINDOW_RECT_CHANGED;
    this->window_id = window_id;
    this->width = width;
    this->height = height;
    this->x = x;
    this->y = y;
  }

  u64 window_id;
  i32 width;
  i32 height;
  i32 x;
  i32 y;
};
} // namespace SK
