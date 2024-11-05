#include "Application.h"

#include "Log.h"

namespace Stak {

Ref<Window> Application::s_Window;
LayerStack Application::s_LayerStack;
bool Application::s_Running = false;

void Application::AddLayer(ApplicationLayer *layer) {
  s_LayerStack.Push(layer);
}

Application::Application(const ApplicationSpec &spec) {
  WindowConfig cfg = {
    spec.WindowWidth,
    spec.WindowHeight,
    spec.WindowTitle
  };
  s_Window = Window::Create(cfg);
  s_Window->SetEventFn([this](Event &event) {
    return this->OnEvent(event);
    });

  if (s_Window) {
    SK_LOG_INFO("Window Created");
  }

  Input::Init(s_Window);

  Renderer::Init(s_Window);

  s_Running = true;
}

void Application::Run() {
  while (s_Running) {
    for (ApplicationLayer *layer : s_LayerStack) {
      layer->Update();
    }

    s_Window->Update();
  }
}

void Application::OnEvent(Event &event) {
  for (ApplicationLayer *layer : s_LayerStack) {
    layer->OnEvent(event);
  }

  switch (event.GetType()) {
  case EventType::WINDOW_CLOSE:
  {
    s_Running = false;
    break;
  }
  case EventType::WINDOW_RESIZED:
  {
    WindowResizeEvent *wre = static_cast<WindowResizeEvent *>(&event);
    SK_LOG_INFO("Window Resized: {0}, {1}", wre->Width, wre->Height);
    break;
  }
  default:
    break;
  }
}

void Application::Close() {
  s_Running = false;
}

} // namespace Stak
