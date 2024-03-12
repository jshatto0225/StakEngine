#pragma once

#include <vector>
#include <memory>
#include <string>

#include "ApplicationLayer.h"
#include "Event.h"
#include "Window.h"

/**
 * App Creation Spec
 */
struct ApplcationSpec {
  std::string window_title;
  i32 window_x;
  i32 window_y;
  i32 window_width;
  i32 window_height;
};

/**
 * Main Application Class
 */
class Application {
public:
  Application(const ApplcationSpec &spec);
  ~Application();

  /**
   * Main Loop
   */
  virtual void run() final;

  /**
   * Event callback
   */
  virtual void onEvent(Event &e) final;

  /**
   * Function to register custom layers
   */
  template<typename T, typename... Args>
  void addLayer(Args&& ...args) {
    static_assert(std::is_base_of<ApplicationLayer, T>().value, "Layer does not inherit from ApplicationLayer");
    layers.push_back(std::make_unique<T>(std::forward<Args>(args)...));
  }

private:
  bool running;
  std::vector<std::unique_ptr<ApplicationLayer>> layers;
  std::unique_ptr<Window> window;
};
