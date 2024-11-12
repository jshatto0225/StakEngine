#include "Application.h"

#include "Log.h"
#include "ImGuiLayer.h"

namespace Stak {

Ref<Window> Application::sWindow;
LayerStack Application::sLayerStack;
bool Application::sRunning = false;
ImGuiLayer *Application::sImGuiLayer;
Ref<Renderer> Application::sRenderer;

void Application::addLayer(IApplicationLayer *layer) {
  sLayerStack.push(layer);
}

Application::~Application() {
  Input::shutdown();
  sLayerStack.clear();

  sImGuiLayer = NULL;
  sRenderer = NULL;
  sWindow = NULL;
}

Application::Application(const ApplicationSpec &spec) {
  WindowConfig cfg = {
    spec.windowWidth,
    spec.windowHeight,
    spec.windowTitle
  };
  sWindow = Window::create(cfg);
  sWindow->setEventFn([this](IEvent &event) {
    return this->onEvent(event);
    });

  if (sWindow) {
    SK_LOG_INFO("Window Created");
  }

  Input::init(sWindow);

  sRenderer = createRef<Renderer>(sWindow);

  sImGuiLayer = new ImGuiLayer(sWindow, sRenderer);

  addLayer(sImGuiLayer);

  sRunning = true;
}

void Application::run() {
  while (sRunning) {
    for (IApplicationLayer *layer : sLayerStack) {
      layer->update();
    }

    sImGuiLayer->beginFrame();
    {
      for (IApplicationLayer *layer : sLayerStack) {
        layer->onImGuiRender();
      }
    }
    sImGuiLayer->endFrame();

    sWindow->update();
  }
}

void Application::onEvent(IEvent &event) {
  for (IApplicationLayer *layer : sLayerStack) {
    layer->onEvent(event);
  }

  switch (event.getType()) {
  case EEventType::WINDOW_CLOSE:
  {
    sRunning = false;
    break;
  }
  case EEventType::WINDOW_RESIZED:
  {
    sRenderer->processWindowChanges(sWindow);
    break;
  }
  default:
    break;
  }
}

void Application::close() {
  sRunning = false;
}

} // namespace Stak
