#include "Camera.h"

#include "Log.h"
#include "StakMath.h"

SceneViewCamera::SceneViewCamera() {
  this->pos = { 0, 0, 0 };
  this->orientation = { 0, 0, 0 };
  this->aspect_ratio = 1;

  this->ortho_near = -1;
  this->ortho_far = 1;
  this->ortho_size = 1;

  calculateView();
  calculateProjection();
  this->view_proj = this->proj * this->view;
}

void SceneViewCamera::setOrthographic(f32 size, f32 near_plane, f32 far_plane) {
  this->type = ProjectionType::ORTHOGRAPHIC;
  this->ortho_size = size;
  this->ortho_near = near_plane;
  this->ortho_far = far_plane;
  this->calculateProjection();
  this->view_proj = this->proj * this->view;
}

void SceneViewCamera::setViewportSize(i32 width, i32 height) {
  this->aspect_ratio = (f32)width / (f32)height;
  this->calculateProjection();
  this->view_proj = this->proj * this->view;
}

void SceneViewCamera::setView(const Vec3 &pos, const Vec3 &orientation) {
  this->pos = pos;
  this->orientation = orientation;
  this->calculateView();
  this->view_proj = this->proj * this->view;
}

void SceneViewCamera::calculateProjection() {
  switch (this->type) {
  case ProjectionType::ORTHOGRAPHIC: {
    f32 left = -this->ortho_size * 0.5 * this->aspect_ratio;
    f32 right = this->ortho_size * 0.5 * this->aspect_ratio;
    f32 bottom = -this->ortho_size * 0.5;
    f32 top = this->ortho_size * 0.5;
    this->proj = OrthographicProjection(left, right, bottom, top, this->ortho_near, this->ortho_far);
    break;
  }
  case ProjectionType::PERSPECTIVE:
    Log::CoreError("Perspective Projection Not Supported");
    break;
  }
}

void SceneViewCamera::calculateView() {
  Mat4 rotation = QuatToMat4(Quat(this->orientation));
  Mat4 transform = TranstationMatrix(-this->pos) * rotation;
  this->view = Inverse(transform);
}

const Mat4 &SceneViewCamera::getViewProj() const {
  return this->view_proj;
}

const Mat4 &SceneViewCamera::getView() const {
  return this->view;
}

const Mat4 &SceneViewCamera::getProj() const {
  return this->proj;
}
