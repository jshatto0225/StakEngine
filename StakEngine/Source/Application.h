#pragma once

#include <vector>

#include "ApplicationLayer.h"
#include "Event.h"
#include "Renderer.h"

namespace Stak {

struct ApplicationSpec {
  const char *windowTitle;
  i32 windowX;
  i32 windowY;
  i32 windowWidth;
  i32 windowHeight;
};

class Application {
public:
  Application(const ApplicationSpec &Spec);

  void Run();

  void OnEvent(Event &event);

  void AddLayer(ApplicationLayer *layer);

  void Close();

private:
  Ref<Window> m_Window;
  LayerStack m_LayerStack;
  bool m_Running;
  Scope<Renderer> m_Renderer;
};

} // namespace Stak
