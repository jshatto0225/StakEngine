#include "Runtime/Runtime.h"

class TestLayer : public sk::ApplicationLayer {
public:
  void start() { SK_CORE_TRACE("Source dir: %s", SOURCE_DIR); }

  void update() {}

  void on_event(sk::Event &e) {}

private:
  Shared<sk::Window> window;
};

void extern_init() {
  sk::application_add_layer(new_unique<TestLayer>());
}
