#pragma once

#include "Event.h"

class ApplicationLayer {
public:
  ApplicationLayer() = default;
  virtual ~ApplicationLayer() {}

  virtual void update() {}
  virtual void onEvent(Event &e) {}
};
