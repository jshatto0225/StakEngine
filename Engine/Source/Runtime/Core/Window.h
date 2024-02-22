#pragma once

#include "Types.h"
#include "Platform.h"

struct SKWindow;

struct _SKContext;

struct _SKWindow {
  bool should_close;
  i32 x;
  i32 y;
  i32 width;
  i32 height;
  const char *title;

  struct {
    void (*window_size_fun)(SKWindow *, i32, i32) = [](SKWindow *, i32, i32) {};
    void (*window_pos_fun)(SKWindow *, i32, i32) = [](SKWindow *, i32, i32) {};
    void (*window_close_fun)(SKWindow *) = [](SKWindow *) {};
  } callbacks;

  SK_PLATFORM_WINDOW_STATE
  SK_PLATFORM_CONTEXT_STATE
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
bool sk_window_should_close(SKWindow *win);
void sk_poll_events();

void sk_set_window_close_callback(SKWindow *win, void(*func)(SKWindow *));
void sk_set_window_size_callback(SKWindow *win, void(*func)(SKWindow *, i32 width, i32 height));
void sk_set_window_posallback(SKWindow *win, void(*func)(SKWindow *, i32 x, i32 y));


