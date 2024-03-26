#pragma once

#include "StakMath.h"

enum projection_type {
  PROJ_TYPE_ORTHOGRAPHIC,
  PROJ_TYPE_PERSPECTIVE
};

struct scene_view_camera {
  mat4 View;
  mat4 Projection;
  mat4 ViewProj;
  projection_type Type = PROJ_TYPE_ORTHOGRAPHIC;
  vec3 Pos = { 0, 0, 0 };
  vec3 Orientation = { 0, 0, 0 };
  f32 AspectRatio = 1;
  f32 OrthoNearPlane = -1;
  f32 OrthoFarPlane = 1;
  f32 OrthoSize = 1;
};

void SetCameraViewportSize(scene_view_camera *Camera, i32 Width, i32 Height);
void RecalculateCameraProjection(scene_view_camera *Camera);
void RecalculateCameraView(scene_view_camera *Camera);
void RecalculateCameraViewProj(scene_view_camera *Camera);
void SetDefaultOrthoCameraSettings(scene_view_camera *Camera);
