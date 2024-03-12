#pragma once

struct NameComponent {
  std::string name;
};

struct TransformComponent {
  Vec3 position;
  Vec3 size;
  Vec3 rotation;
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