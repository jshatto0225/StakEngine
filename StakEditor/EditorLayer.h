#pragma once

#include "../StakEngine/StakEngine.h"

struct editor_data {
  texture2d *Tex1;
  texture2d *Tex2;
  texture2d *Tex3;
  scene_view_camera Cam;
  i32 WindowWidth;
  i32 WindowHeight;

  f32 QuadRotation;
  vec2 Vec1;
  vec2 Vec2;
  vec2 Vec3;
};

void *EditorLayerInit();
void EditorLayerShutdown(void **);
void EditorLayerUpdate(void *);
void EditorLayerOnEvent(void *, void *, const event *);
