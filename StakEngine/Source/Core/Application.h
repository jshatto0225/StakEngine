#pragma once

#include <vector>

#include "ApplicationLayer.h"
#include "Event.h"
#include "Renderer.h"
#include "Input.h"

namespace Stak {

struct ApplicationSpec {
  const char *WindowTitle;
  i32 WindowWidth;
  i32 WindowHeight;
};

class Application {
public:
  Application(const ApplicationSpec &Spec);

  void Run();
  void OnEvent(Event &event);
  void AddLayer(ApplicationLayer *layer);
  void Close();

  const Scope<InputManager> &Input() const { return m_InputManager; }

private:
  Ref<Window> m_Window;
  LayerStack m_LayerStack;
  bool m_Running;
  Scope<Renderer> m_Renderer;
  Scope<InputManager> m_InputManager;
};

} // namespace Stak
