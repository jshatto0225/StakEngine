#pragma once

#include <Runtime/Runtime.h>

/**
 * Custom editor layer
 */
class EditorLayer : public sk::ApplicationLayer {
public:
  EditorLayer(i32 window_width,
              i32 window_height);
  ~EditorLayer();

  void update() override;
  void onEvent(sk::Event &e) override;

private:
  Ref<sk::Texture2D> tex;
  Ref<sk::Texture2D> tex2;
  Ref<sk::Texture2D> tex3;
  sk::SceneViewCamera cam;
};
