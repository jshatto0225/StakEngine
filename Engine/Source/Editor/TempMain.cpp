#include "Runtime/Runtime.h"

class TempLayer : public ApplicationLayer {
  void update() override {
    Vec2 pos = Input::GetMousePos();
    f32 z = std::sqrt(pos.x * pos.x + pos.y * pos.y);
    f32 scale = std::sqrt(1280 * 1280 + 720 * 720);
    RenderApi::SetClearColor((f32)pos.x / 1280, (f32)pos.y / 720, z / scale, 1);
    RenderApi::Clear();
  }
};

class TempApp : public Application {
public:
  TempApp() {
    this->addLayer<TempLayer>();
  }
};

std::unique_ptr<Application> GetApp() {
  return std::make_unique<TempApp>();
}
