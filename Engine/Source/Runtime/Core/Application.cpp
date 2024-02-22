#include "Application.h"

#include <vector>

#include "Log.h"
#include "Platform.h"
#include "Renderer.h"

_SKApplication _app;

void sk_app_add_layer(SKAppLayer *layer) {
  _app.layers.push_back(layer);
}

void sk_app_init() {
  sk_debug_core_trace("Initializing Platform");
  sk_platform_init();

  sk_debug_core_trace("New Window");
  _app.window = sk_create_window({ 100, 100, 1280, 720, "WINDOW" });
  sk_set_window_close_callback(_app.window, [](SKWindow *win) {
    SKEvent e = {};
    e.type = SK_WINDOW_CLOSE;
    e.win_close_event.window = win;
    sk_app_on_event(e);
  });
  sk_set_window_size_callback(_app.window, [](SKWindow *win, i32 width, i32 height) {
    SKEvent e = {};
    e.type = SK_WINDOW_RESIZED;
    e.win_resize_event.window = win;
    e.win_resize_event.width = width;
    e.win_resize_event.height = height;
    sk_app_on_event(e);
  });
  sk_set_window_pos_callback(_app.window, [](SKWindow *win, i32 x, i32 y) {
    SKEvent e = {};
    e.type = SK_WINDOW_MOVED;
    e.win_move_event.window = win;
    e.win_move_event.x = x;
    e.win_move_event.y = y;
    sk_app_on_event(e);
  });
  sk_make_context_current(_app.window);
  
  sk_debug_core_trace("Initializing renderer");
  sk_render_command_init(_app.window);
  _app.running = true;

  for (SKAppLayer *layer : _app.layers) {
    layer->start();
  }
}

void sk_app_run() {
  while (_app.running) {
    sk_poll_events();
    sk_window_swap_buffers(_app.window);

    for (SKAppLayer *layer : _app.layers) {
      layer->update();
    }
  }
}

void sk_app_on_event(SKEvent &e) {
  if (_app.running) {
    for (SKAppLayer *layer : _app.layers) {
      layer->on_event(e);
    }
    switch (e.type) {
    case SK_WINDOW_CLOSE:
      if (e.win_close_event.window == _app.window) {
        sk_debug_core_trace("Main Window Closed");
        sk_app_shutdown();
      }
      break;

    case SK_WINDOW_RESIZED:
      sk_debug_core_trace("Window size changed");
      sk_render_command_set_viewport(e.win_resize_event.window);
      break;

    case SK_WINDOW_MOVED:
      sk_debug_core_trace("Window pos changed");
      sk_render_command_set_viewport(e.win_move_event.window);
      break;

    default:
      break;
    }
  }
}

void sk_app_shutdown() {
  if (_app.running) {
    sk_debug_core_trace("Application Shutdown");
    _app.running = false;
    for (SKAppLayer *layer : _app.layers) {
      layer->end();
      delete layer;
    }
    _app.layers.clear();
    sk_render_command_shutdown();
    sk_destroy_window(_app.window);
    sk_platform_shutdown();
  }
}

