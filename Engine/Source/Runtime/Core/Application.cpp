#include "Application.h"

#include <memory>

#include "Log.h"
#include "Platform.h"
#include "Renderer.h"

Application::Application() {
  this->window = MakeUnique<Window>(WindowConfig({ 100, 100, 1280, 720, "Window" }));
  this->window->setEventCallback(BIND_METHOD(Application::onEvent));
  
  Renderer::Init();

  this->running = true;
  Log::CoreInfo("Application Initialized");
}

void Application::run() {
  while (this->running) {
    for (const Unique<ApplicationLayer> &layer : this->layers) {
      layer->update();
    }
    this->window->update();
  }
}

void Application::onEvent(Event &e) {
  if (this->running) {
    for (const Unique<ApplicationLayer> &layer : this->layers) {
      layer->onEvent(e);
    }
    switch (e.type) {
    case WINDOW_CLOSE:
      Log::CoreTrace("Window Closed");
      this->running = false;
      break;

    case WINDOW_RESIZED:
      Renderer::OnWindowResize(e.win_resize_event.width, e.win_resize_event.height);
      Log::CoreTrace("Viewport: 0, 0, %d, %d", e.win_resize_event.width, e.win_resize_event.height);
      break;

    default:
      break;
    }
  }
}

Application::~Application() {
  Log::CoreTrace("Application Shutdown");

  Renderer::Shutdown();
}
