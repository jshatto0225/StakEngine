#pragma once

#include <Runtime/Runtime.h>

/**
 * Custom editor layer
 */
class EditorLayer : public ApplicationLayer {
public:
  EditorLayer(i32 window_width,
              i32 window_height);
  ~EditorLayer();

  void update() override;
  void onEvent(Event &e) override;

private:
  Ref<Texture2D> tex;
  Ref<Texture2D> tex2;
  Ref<Texture2D> tex3;
  SceneViewCamera cam;
};