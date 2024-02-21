#if defined(SK_WINDOWS)

#include "Window.h"

SKWindow *sk_create_window(const SKWindowConfig &config) {
  return 0;
}

void sk_destroy_window(SKWindow *win) {

}

void sk_set_window_pos(SKWindow *win, i32 x, i32 y) {

}

void sk_set_window_size(SKWindow *win, i32 width, i32 height) {

}

void sk_get_window_pos(SKWindow *win, i32 *x, i32 *y) {

}

void sk_get_window_size(SKWindow *win, i32 *width, i32 *height) {

}

void sk_window_should_close(SKWindow *win) {

}

void sk_poll_events() {

}

#endif
