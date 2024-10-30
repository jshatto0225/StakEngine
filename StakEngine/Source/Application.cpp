#include "Application.h"

#include "Renderer.h"
#include "Window.h"

namespace Stak {

void Application::AddLayer(ApplicationLayer *layer) {
  m_LayerStack.Push(layer);
}

Application::Application(const ApplicationSpec &spec) {
  WindowConfig cfg = {spec.windowX, spec.windowY, spec.windowWidth,
                      spec.windowHeight, spec.windowTitle};
  //m_Window = Window::Create(cfg);
  //m_Window->SetEventFn([this](Event &event) { return this->OnEvent(event); });

  //m_Renderer = Renderer::Create(m_Window);

  m_Running = true;
}

void Application::Run() {
  while (m_Running) {
    for (ApplicationLayer *layer : m_LayerStack) {
      layer->Update();
    }

    //m_Window->Update();

    //m_Renderer->DrawFrame();
  }

  //m_Renderer->WaitForGpu();
}

void Application::OnEvent(Event &event) {
  if (m_Running) {
    for (ApplicationLayer *layer : m_LayerStack) {
      layer->OnEvent(event);
    }

    switch (event.GetType()) {
    case EventType::WINDOW_CLOSE: {
      m_Running = false;
      break;
    }

    case EventType::WINDOW_RESIZED: {
      WindowResizeEvent *wre = (WindowResizeEvent *)&event;
      //m_Renderer->SetViewport(0, 0, wre->Width, wre->Height);
      break;
    }

    default:
      break;
    }
  }
}

void Application::Close() { m_Running = false; }

} // namespace Stak
