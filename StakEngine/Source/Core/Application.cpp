#include "Application.h"

#include "Log.h"
#include "ImGuiLayer.h"

namespace Stak {

Ref<Window> Application::s_Window;
LayerStack Application::s_LayerStack;
bool Application::s_Running = false;

void Application::addLayer(ApplicationLayer *layer) {
  s_LayerStack.push(layer);
}

Application::Application(const ApplicationSpec &spec) {
  WindowConfig cfg = {
    spec.windowWidth,
    spec.windowHeight,
    spec.windowTitle
  };
  s_Window = Window::create(cfg);
  s_Window->setEventFn([this](Event &event) {
    return this->onEvent(event);
    });

  if (s_Window) {
    SK_LOG_INFO("Window Created");
  }

  Input::init(s_Window);

  Renderer::init(s_Window);

  addLayer(new ImGuiLayer(s_Window));

  s_Running = true;
}

void Application::run() {
  while (s_Running) {
    for (ApplicationLayer *layer : s_LayerStack) {
      layer->update();
    }

    s_Window->update();
  }
}

void Application::onEvent(Event &event) {
  for (ApplicationLayer *layer : s_LayerStack) {
    layer->onEvent(event);
  }

  switch (event.getType()) {
  case EventType::WINDOW_CLOSE:
  {
    s_Running = false;
    break;
  }
  case EventType::WINDOW_RESIZED:
  {
    WindowResizeEvent *wre = static_cast<WindowResizeEvent *>(&event);
    SK_LOG_INFO("Window Resized: {}, {}", wre.width, wre.height);
    break;
  }
  default:
    break;
  }
}

void Application::close() {
  s_Running = false;
}

} // namespace Stak
