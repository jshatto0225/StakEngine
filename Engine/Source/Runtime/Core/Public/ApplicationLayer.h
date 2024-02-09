#pragma once

#include "Event.h"

namespace sk {
class ApplicationLayer {
public:
  virtual void start() {}
  virtual void end() {}
  virtual void update() {}
  virtual void on_event(Event &e) {}
};
} // namespace SK
