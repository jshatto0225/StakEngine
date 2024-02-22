#include "Application.h"

#include <vector>

#include "AppLayer.h"
#include "Log.h"
#include "Window.h"
#include "Platform.h"
#include "Context.h"
#include "Renderer.h"

// TODO: Remove
#include "OpenGLBindings.h"

static bool running;
static std::vector<SKAppLayer *> layers;
//static InputManager *input_manager;
static SKWindow *window;

void sk_app_add_layer(SKAppLayer *layer) {
  layers.push_back(layer);
}

void sk_app_init() {
  SK_CORE_TRACE("Initializing Platform\n");
  sk_platform_init();
  SK_CORE_TRACE("New Window\n");
  window = sk_create_window({ 100, 100, 1280, 720, "WINDOW" });
  sk_set_window_close_callback(window, [](SKWindow *win) {
    SKEvent e = {};
    e.type = SK_WINDOW_CLOSE;
    e.win_close_event.window = win;
    sk_app_on_event(e);
  });
  sk_make_context_current(window);
  
  //SK_CORE_TRACE("Initializing renderer\n");
  //sk_renderer_init(window);
  //SK_CORE_TRACE("New input manager\n");
  //input_manager = sk_create_input_manager();
  running = true;

  for (SKAppLayer *layer : layers) {
    layer->start();
  }
}

void sk_app_run() {
  while (running) {
    sk_poll_events();

    // TODO: move
    glClearColor(0, 255, 255, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    sk_window_swap_buffers(window);

    for (SKAppLayer *layer : layers) {
      layer->update();
    }
  }
}

void sk_app_on_event(SKEvent &e) {
  if (running) {
    for (SKAppLayer *layer : layers) {
      layer->on_event(e);
    }
    switch (e.type) {
    case SK_WINDOW_CLOSE: {
      if (e.win_close_event.window == window) {
        sk_app_shutdown();
      }
      break;
    }
    default:
    {
      break;
    }
    }
  }
}

void sk_app_shutdown() {
  if (running) {
    //SK_CORE_WARN("Application Shutdown\n");
    running = false;
    for (SKAppLayer *layer : layers) {
      layer->end();
      delete layer;
    }
    layers.clear();
    //sk_renderer_shutdown();
    sk_destroy_window(window);
    sk_platform_shutdown();
  }
}

