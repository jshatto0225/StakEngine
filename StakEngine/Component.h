#pragma once

#include "Types.h"
#include "StakMath.h"
#include "Camera.h"

struct NameComponent {
  const char *name;
};

struct TransformComponent {
  vec3 position;
  vec3 size;
  vec3 rotation;
};

enum class CameraProjection {
  ORTHOGRAPHIC,
  PERSPECTIVE
};

struct CameraComponent {
  CameraProjection proj;
  f32 fov;
  f32 ortho_far_clip_plane;
  f32 ortho_near_clip_plane;
  f32 ortho_width_in_world_coords;
};