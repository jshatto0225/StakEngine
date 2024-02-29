#pragma once

#include <Runtime/Runtime.h>

class EditorLayer : public ApplicationLayer {
public:
  EditorLayer();
  ~EditorLayer();

  void update() override;
  void onEvent(Event &e) override;
};