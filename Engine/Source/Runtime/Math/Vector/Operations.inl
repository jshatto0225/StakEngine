#include "Operations.h"

template<typename T>
T Dot(const Vec<T, 3> &v1, const Vec<T, 3> &v2) {
  return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

template<typename T>
Vec<T, 3> Normalize(const Vec<T, 3> &v) {
  T mag = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
  return Vec<T, 3>(v.x / mag, v.y / mag, v.z / mag);
}

template<typename T>
Vec<T, 3> Cross(const Vec<T, 3> &v1, const Vec<T, 3> &v2) {
  return Vec<T, 3>(
    v1.y * v2.z - v1.z * v2.y,
    v1.z * v2.x - v1.x * v2.z,
    v1.x * v2.y - v1.y * v2.x
  );
}

Mat4 ViewMatrix(const Vec3 &pos, const Vec3 &up, const Vec3 &forward) {
  Vec3 z_axis = Normalize(forward);
  Vec3 x_axis = Normalize(Cross(up, z_axis));
  Vec3 y_axis = Cross(z_axis, x_axis);

  Mat4 view(
    x_axis.x, y_axis.x, z_axis.z, 0,
    x_axis.y, y_axis.y, z_axis.z, 0,
    x_axis.z, y_axis.z, z_axis.z, 0,
    Dot(x_axis, pos), Dot(y_axis, pos), Dot(z_axis, pos), 1);
  return view;
}

Mat4 OrthographicProjection(f32 left, f32 right, f32 bottom, f32 top, f32 near_plane, f32 far_plane) {
  Mat4 m(0);
  m(0, 0) = 2.0f / (right - left);
  m(1, 1) = 2.0f / (top - bottom);
  m(2, 2) = -2.0f / (far_plane - near_plane);
  m(3, 0) = -(right + left) / (right - left);
  m(3, 1) = -(top + bottom) / (top - bottom);
  m(3, 2) = -(far_plane + near_plane) / (far_plane - near_plane);
  m(3, 3) = 1.0f;
  return m;
}
