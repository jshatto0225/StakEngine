#pragma once

#include <vector>

#include "ApplicationLayer.h"
#include "Event.h"
#include "Renderer.h"
#include "Input.h"
#include "ImGuiLayer.h"

namespace Stak {

struct ApplicationSpec {
  const char *windowTitle;
  i32 windowWidth;
  i32 windowHeight;
};

class Application {
public:
  Application(const ApplicationSpec &Spec);

  static void run();
  static void onEvent(Event &event);
  static void addLayer(ApplicationLayer *layer);
  static void close();

private:
  static Ref<Window> sWindow;
  static Ref<Renderer> sRenderer;
  static LayerStack sLayerStack;
  static bool sRunning;
  static ImGuiLayer *sImGuiLayer;
};

} // namespace Stak
