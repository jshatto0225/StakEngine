#include "Application.h"

#include "AppLayer.h"
#include "Log.h"
#include "Window.h"
//#include "PlatformManager.h"
//#include "Renderer.h"

static bool running;
static std::vector<AppLayer *> layers;
//static InputManager *input_manager;
static SKWindow *window;

void sk_app_add_layer(AppLayer *layer) {
  layers.push_back(layer);
}

void sk_app_init() {
  SK_CORE_TRACE("New Window\n");
  window = sk_create_window({ 100, 100, 1280, 720, "WINDOW" });
  //SK_CORE_TRACE("Initializing renderer\n");
  //sk_renderer_init(window);
  //SK_CORE_TRACE("New input manager\n");
  //input_manager = sk_create_input_manager();
  running = true;

  for (AppLayer *layer : layers) {
    layer->start();
  }
}

void sk_app_run() {
  while (running) {
    sk_poll_events();

    for (AppLayer *layer : layers) {
      layer->update();
    }
  }
}

void sk_app_on_event(Event &e) {
  if (running) {
    for (AppLayer *layer : layers) {
      layer->on_event(e);
    }
    /*
    switch (e.type) {
    case EventType::WINDOW_CLOSE: {
      WindowCloseEvent *wce = (WindowCloseEvent *)&e;
      if (wce->window_id == window->get_id()) {
        sk_app_shutdown();
      }
      break;
    }
    default:
    {
      break;
    }
    }
    */
  }
}

void sk_app_shutdown() {
  if (running) {
    //SK_CORE_WARN("Application Shutdown\n");
    running = false;
    for (AppLayer *layer : layers) {
      layer->end();
      delete layer;
    }
    layers.clear();
    //sk_renderer_shutdown();
    sk_destroy_window(window);
  }
}

