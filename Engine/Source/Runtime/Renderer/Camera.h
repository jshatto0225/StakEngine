#pragma once

#include "StakMath.h"

namespace sk {
/**
 * Type of prohection a camera uses
 */
enum class ProjectionType {
  ORTHOGRAPHIC,
  PERSPECTIVE
};

/**
 * This class is for a camera that the editor should use to look at the scene
 * NOTE: This is different from a camera component that any final games should be using
 */
class SceneViewCamera {
public:
  SceneViewCamera();

  /**
   * Sets camera as orthographic
   *
   * near_plane and far_plane are clipping planes
   *
   * Size is the relative size of the cameras viewport.
   * Determines what coordinate space
   */
  void setOrthographic(f32 size, f32 near_plane, f32 far_plane);

  /**
  * Set the viewport size of the camera
  * Usually screen size
  */
  void setViewportSize(i32 width, i32 height);

  /**
   * Set the view (pos and orientation) of the camera
   */
  void setView(const Vec3 &pos, const Vec3 &orientation);

  /**
   * Calculate the cameras projection matrix
   */
  void calculateProjection();

  /**
   * Calculate the cameras view matrix
   */
  void calculateView();

  /**
   * Getters
   */
  const Mat4 &getViewProj() const;
  const Mat4 &getView() const;
  const Mat4 &getProj() const;

private:
  Mat4 view; /// View matrix
  Mat4 proj; /// Proj matrix
  Mat4 view_proj; /// proj * view

  ProjectionType type; /// Projection type
  Vec3 pos; /// world position
  Vec3 orientation; /// world orientation
  f32 aspect_ratio; /// aspect ratio of viewport

  f32 ortho_near; /// near clipping plane
  f32 ortho_far; /// far clipping plane
  f32 ortho_size; /// world viewport size
};

} // namespace sk
