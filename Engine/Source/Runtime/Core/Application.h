#pragma once

#include <vector>

#include "ApplicationLayer.h"
#include "Event.h"
#include "Window.h"

namespace sk {
class Application {
public:
  Application();
  ~Application();

  virtual void run() final;
  virtual void on_event(Event &e) final;

  template<typename T>
  void add_layer() {
    static_assert(std::is_base_of<ApplicationLayer, T>().value, "Layer does not inherit from ApplicationLayer");
    layers.push_back(new T());
  }

private:
  bool running;
  std::vector<ApplicationLayer *> layers;
  Window window;
};
}
