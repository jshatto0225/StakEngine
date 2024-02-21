#pragma once

//#include "Event.h"
#include "Types.h"

#include "Platform.h"

struct SKWindow;

struct _SKWindow {
  bool open;
  i32 x;
  i32 y;
  i32 width;
  i32 height;
  const char *title;

  struct {
    //TODO: make callbacks
  } callbacks;

  SK_PLATFORM_WINDOW_STATE
};

struct SKWindowConfig {
  i32 x;
  i32 y;
  i32 width;
  i32 height;
  const char* title;
};

SKWindow *sk_create_window(const SKWindowConfig &config);
void sk_destroy_window(SKWindow *win);
void sk_set_window_pos(SKWindow *win, i32 x, i32 y);
void sk_set_window_size(SKWindow *win, i32 width, i32 height);
void sk_get_window_pos(SKWindow *win, i32 *x, i32 *y);
void sk_get_window_size(SKWindow *win, i32 *width, i32 *height);
void sk_window_should_close(SKWindow *win);
void sk_poll_events();

/*
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

  virtual void *get_handle() = 0;

  virtual void set_size_and_pos(i32 x, i32 y, i32 width, i32 height) = 0;
  virtual void update() = 0;
  virtual void close() = 0;

  virtual void generate_event(EventType e) final;

private:
  u64 id;
  static u64 current_id;
};
*/

