#pragma once

#include <Runtime/Runtime.h>

class EditorLayer : public ApplicationLayer {
public:
  EditorLayer();
  ~EditorLayer();

  void update() override;
  void onEvent(Event &e) override;

private:
  Shared<Texture2D> tex;
  Shared<Texture2D> tex2;
  Shared<Texture2D> tex3;
};