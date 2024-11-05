#pragma once

#include "ApplicationLayer.h"
#include "Window.h"

namespace Stak {

class ImGuiLayer : public ApplicationLayer {
public:
  ImGuiLayer(Ref<Window> window);
  ~ImGuiLayer();

  void OnAttach();
  void OnDetach();
  void Update();
  void OnEvent(Event &event);
};

} // namespace Stak