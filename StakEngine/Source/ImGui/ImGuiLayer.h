#pragma once

#include "ApplicationLayer.h"
#include "Window.h"

namespace Stak {

class ImGuiLayer : public ApplicationLayer {
public:
  ImGuiLayer(Ref<Window> window);
  ~ImGuiLayer();

  void onAttach();
  void onDetach();
  void update();
  void onEvent(Event &event);
};

} // namespace Stak
