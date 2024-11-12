#pragma once

#include "Types.h"

class Window;

enum class EEventType {
  NONE,
  WINDOW_RESIZED,
  WINDOW_MOVED,
  WINDOW_CLOSE,
};

class IEvent {
public:
  IEvent() = default;

  virtual EEventType getType() const = 0;
};

class WindowCloseEvent : public IEvent {
public:
  inline EEventType getType() const  { return EEventType::WINDOW_CLOSE; }
};

class WindowResizeEvent : public IEvent {
public:
  WindowResizeEvent(i32 width, i32 height) : width(width), height(height) {}
  inline EEventType getType() const  { return EEventType::WINDOW_RESIZED; }

  const i32 width;
  const i32 height;
};

class WindowMovedEvent : public IEvent {
public:
  WindowMovedEvent(i32 x, i32 y) : x(x), y(y) {}
  inline EEventType getType() const  { return EEventType::WINDOW_MOVED; }

  const i32 x;
  const i32 y;
};

