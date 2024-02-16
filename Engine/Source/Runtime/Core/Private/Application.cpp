#include "Application.h"

#include "Log.h"
#include "PlatformManager.h"
#include "Renderer.h"

namespace sk {
static bool running;
static std::vector<Shared<ApplicationLayer>> application_layers;
static Unique<InputManager> input_manager;
static Unique<Window> window;

void application_add_layer(Unique<ApplicationLayer> layer) {
  application_layers.push_back(std::move(layer));
}

void application_init() {
  SK_CORE_TRACE("New Window");
  window = new_window("Stak Applciation", 100, 100, 1280, 720);
  SK_CORE_TRACE("Initializing renderer");
  renderer_init(window);
  SK_CORE_TRACE("New input manager");
  input_manager = new_input_manager();
  running = true;

  for (Shared<ApplicationLayer> layer : application_layers) {
    layer->start();
  }
}

void application_run() {
  while (running) {
    window->update();

    for (Shared<ApplicationLayer> layer : application_layers) {
      layer->update();
    }
  }
}

void application_on_event(Event &e) {
  if (running) {
    for (Shared<ApplicationLayer> layer : application_layers) {
      layer->on_event(e);
    }

    switch (e.type) {
    case WINDOW_CLOSE: {
      WindowCloseEvent *wce = (WindowCloseEvent *)&e;
      if (wce->window_id == window->get_id()) {
        application_shutdown();
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

void application_shutdown() {
  if (running) {
    SK_CORE_WARN("Application Shutdown");
    running = false;
    for (Shared<ApplicationLayer> layer : application_layers) {
      layer->end();
      //layer.reset();
    }
    application_layers.clear();
    renderer_shutdown();
    //window.reset();
  }
}
} // namespace sk
