#pragma once

#include "Event.h"
#include "Types.h"

namespace sk {
class Window {
public:
  Window();
  ~Window() {}

  virtual void set_x(i32 x) = 0;
  virtual void set_y(i32 y) = 0;
  virtual void set_width(i32 width) = 0;
  virtual void set_height(i32 height) = 0;
  virtual i32 get_width() = 0;
  virtual i32 get_height() = 0;
  virtual i32 get_x() = 0;
  virtual i32 get_y() = 0;
  virtual f32 get_aspect() = 0;

  virtual bool is_open() = 0;
  virtual u64 get_id() final;

  virtual void set_size_and_pos(i32 x, i32 y, i32 width, i32 height) = 0;
  virtual void update() = 0;
  virtual void close() = 0;

  virtual void generate_event(EventType e) final;

private:
  u64 id;
  static u64 current_id;
};
} // namespace SK
