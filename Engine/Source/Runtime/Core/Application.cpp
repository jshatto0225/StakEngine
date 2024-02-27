#include "Application.h"

#include "Log.h"
#include "Platform.h"
#include "Renderer.h"

namespace sk {
Application::Application() 
  : window({ 100, 100, 1280, 720, "WINDOW" }), running(true) {
  this->window.set_event_callback(std::bind(&Application::on_event, this, std::placeholders::_1));
}

void Application::run() {
  while (this->running) {
    for (ApplicationLayer *layer : this->layers) {
      layer->update();
    }
    poll_events();
    this->window.swap_buffers();
  }
}

void Application::on_event(Event &e) {
  if (this->running) {
    for (ApplicationLayer *layer : this->layers) {
      layer->on_event(e);
    }
    switch (e.type) {
    case WINDOW_CLOSE:
      sk_debug_core_trace("Window Closed");
      if (e.win_close_event.window == &this->window) {
        sk_debug_core_trace("Main Window Closed");
        this->running = false;
      }
      break;

    case WINDOW_RESIZED:
      sk_debug_core_trace("Window size changed");
      break;

    case WINDOW_MOVED:
      sk_debug_core_trace("Window position changed");
      break;

    default:
      break;
    }
  }
}

Application::~Application() {
  sk_debug_core_trace("Application Shutdown");
  for (ApplicationLayer *layer : this->layers) {
    layer->end();
    delete layer;
  }
}
} // namespace sk



