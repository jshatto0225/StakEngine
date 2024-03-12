#include "Application.h"

#include <memory>

#include "Log.h"
#include "Platform.h"
#include "Renderer.h"

namespace sk {
Application::Application(const ApplcationSpec &spec) {
  this->window = std::make_unique<Window>(WindowConfig({
    spec.window_x,
    spec.window_y,
    spec.window_width,
    spec.window_height,
    spec.window_title.c_str() }));
  this->window->setEventCallback(BIND_METHOD(Application::onEvent));

  Renderer::Init();

  this->running = true;
  Log::CoreInfo("Application Initialized");
}

void Application::run() {
  while (this->running) {
    for (const std::unique_ptr<ApplicationLayer> &layer : this->layers) {
      layer->update();
    }
    this->window->update();
  }
}

void Application::onEvent(Event &e) {
  if (this->running) {
    for (const std::unique_ptr<ApplicationLayer> &layer : this->layers) {
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

} // namespace sk
