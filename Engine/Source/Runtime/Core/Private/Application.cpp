#include "Application.h"

#include "Log.h"
#include "PlatformManager.h"
#include "Renderer.h"

namespace sk {
bool Application::running;
std::vector<ApplicationLayer *> Application::application_layers;
Unique<InputManager> Application::input_manager;
Unique<Window> Application::window;

void Application::init() {
  window = PlatformManager::new_window("Stak Applciation", 100, 100, 1280, 720);
  Renderer::init(window);
  input_manager = PlatformManager::new_input_manager();
  running = true;

  for (ApplicationLayer *layer : application_layers) {
    layer->start();
  }
}

void Application::run() {
  while (running) {
    window->update();

    for (ApplicationLayer *layer : application_layers) {
      layer->update();
    }
  }
}

void Application::on_event(Event &e) {
  if (running) {
    for (ApplicationLayer *layer : application_layers) {
      layer->on_event(e);
    }

    switch (e.type) {
    case WINDOW_CLOSE: {
      WindowCloseEvent *wce = (WindowCloseEvent *)&e;
      if (wce->window_id == window->get_id()) {
        Application::shutdown();
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

void Application::shutdown() {
  if (running) {
    SK_CORE_WARN("Application Shutdown");
    running = false;
    for (ApplicationLayer *layer : application_layers) {
      layer->end();
      delete layer;
    }
    application_layers.clear();
    Renderer::shutdown();
    window.reset();
  }
}
} // namespace SK
