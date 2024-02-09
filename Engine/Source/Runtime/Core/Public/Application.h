#pragma once

#include <vector>

#include "ApplicationLayer.h"
#include "Event.h"
#include "InputManager.h"
#include "Window.h"

namespace sk {
class Application final {
public:
  static void init();
  static void run();
  static void shutdown();

  static void on_event(Event &e);

  template <typename LayerType> static void register_layer() {
    static_assert(std::is_base_of<ApplicationLayer, LayerType>::value,
                  "All layers must inherit from ApplicationLayer");
    application_layers.push_back(new LayerType);
  }

private:
  static bool running;
  // TODO: Remove raw pointer
  static std::vector<ApplicationLayer *> application_layers;
  static Unique<InputManager> input_manager;
  static Unique<Window> window;
};
} // namespace SK
