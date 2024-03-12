#pragma once

#include "Event.h"

/**
 * Layers used to build an application
 */
class ApplicationLayer {
public:
  ApplicationLayer() = default;
  virtual ~ApplicationLayer() {}

  /**
   * Called every update
   */
  virtual void update() {}

  /**
   * Called on every event
   */
  virtual void onEvent(Event &e) {}
};
