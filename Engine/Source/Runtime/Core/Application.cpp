#include "Application.h"

#include "Log.h"
#include "Platform.h"
#include "Renderer.h"

Application::Application() {
  this->window = std::make_unique<Window>(WindowConfig({ 100, 100, 1280, 720, "Window" }));
  this->window->setEventCallback(std::bind(&Application::onEvent, this, std::placeholders::_1));

  // TODO: Renderer
  RenderApi::Init();

  this->running = true;
  Log::CoreInfo("Application Initialized");
}

void Application::run() {
  while (this->running) {
    for (const std::unique_ptr<ApplicationLayer>& layer : this->layers) {
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
      if (e.win_close_event.window == this->window.get()) {
        this->running = false;
      }
      break;
    default:
      break;
    }
  }
}

Application::~Application() {
  Log::CoreTrace("Application Shutdown");

  // TODO: Renderer
  RenderApi::Shutdown();
}
