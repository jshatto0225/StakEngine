#pragma once

#include <vector>
#include <memory>

#include "ApplicationLayer.h"
#include "Event.h"
#include "Window.h"

class Application {
public:
  Application();
  ~Application();

  virtual void run() final;
  virtual void onEvent(Event &e) final;

  template<typename T>
  void addLayer() {
    static_assert(std::is_base_of<ApplicationLayer, T>().value, "Layer does not inherit from ApplicationLayer");
    layers.push_back(MakeUnique<T>());
  }

private:
  bool running;
  std::vector<Unique<ApplicationLayer>> layers;
  Unique<Window> window;
};
