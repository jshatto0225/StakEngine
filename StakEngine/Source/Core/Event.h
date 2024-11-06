#pragma once

#include "Types.h"

class Window;

enum class EventType {
  NONE,
  WINDOW_RESIZED,
  WINDOW_MOVED,
  WINDOW_CLOSE,
};

class Event {
public:
  Event() = default;

  virtual EventType getType() = 0;
};

class WindowCloseEvent : public Event {
public:
  inline EventType getType() { return EventType::WINDOW_CLOSE; }
};

class WindowResizeEvent : public Event {
public:
  WindowResizeEvent(i32 width, i32 height) : width(width), height(height) {}
  inline EventType getType() { return EventType::WINDOW_RESIZED; }

  const i32 width;
  const i32 height;
};

class WindowMovedEvent : public Event {
public:
  WindowMovedEvent(i32 x, i32 y) : x(x), y(y) {}
  inline EventType getType() { return EventType::WINDOW_MOVED; }

  const i32 x;
  const i32 y;
};

