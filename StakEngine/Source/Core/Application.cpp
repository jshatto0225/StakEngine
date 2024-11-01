#include "Application.h"

#include "Renderer.h"
#include "Window.h"
#include "Log.h"
#include "Input.h"

namespace Stak {

void Application::AddLayer(ApplicationLayer *layer) {
  m_LayerStack.Push(layer);
}

Application::Application(const ApplicationSpec &spec) {
  WindowConfig cfg = {
    spec.WindowWidth,
    spec.WindowHeight, 
    spec.WindowTitle
  };
  m_Window = Window::Create(cfg);
  m_Window->SetEventFn([this](Event &event) { 
    return this->OnEvent(event);
    });

  if (m_Window) {
    SK_LOG_INFO("Window Created");
  }

  m_InputManager = InputManager::Create(m_Window);

  m_Renderer = CreateScope<Renderer>(m_Window);

  m_Running = true;
}

void Application::Run() {
  while (m_Running) {
    for (ApplicationLayer *layer : m_LayerStack) {
      layer->Update();
    }

    m_Window->Update();

    //m_Renderer->DrawFrame();
  }

  //m_Renderer->WaitForGpu();
}

void Application::OnEvent(Event &event) {
  for (ApplicationLayer *layer : m_LayerStack) {
    layer->OnEvent(event);
  }

  switch (event.GetType()) {
  case EventType::WINDOW_CLOSE:
  {
    m_Running = false;
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
  m_Running = false;
}

} // namespace Stak
