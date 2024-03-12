#include "StakMath.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

Mat4 ViewMatrix(const Vec3 &pos, const Vec3 &up, const Vec3 &forward) {
  return Mat4(1.0f);
}

Mat4 OrthographicProjection(f32 left, f32 right, f32 bottom, f32 top, f32 near_plane, f32 far_plane) {
  return glm::ortho(left, right, bottom, top, near_plane, far_plane);
}

Mat4 Mult(const Mat4 m1, const Mat4 m2) {
  return m1 * m2;
}

Mat4 TranstationMatrix(const Vec3 &pos) {
  return glm::translate(glm::mat4(1.0f), pos);
}

Mat4 Inverse(const Mat4 &mat) {
  return glm::inverse(mat);
}

Mat4 QuatToMat4(const Quat &q) {
  return glm::toMat4(q);
}

Mat4 ScaleMatrix(const Vec3 &scale) {
  return glm::scale(glm::mat4(1.0f), scale);
}