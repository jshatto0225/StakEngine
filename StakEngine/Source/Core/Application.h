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

  static void Run();
  static void OnEvent(Event &event);
  static void AddLayer(ApplicationLayer *layer);
  static void Close();

private:
  static Ref<Window> s_Window;
  static LayerStack s_LayerStack;
  static bool s_Running;
};

} // namespace Stak
