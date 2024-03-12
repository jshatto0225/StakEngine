#pragma once

// TODO: No more glm
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Types.h"

/**
 * Temporary type aliasing while actual types get fixed and reimplemented
 */
using Mat4 = glm::mat4;
using Mat3 = glm::mat3;
using Mat2 = glm::mat2;
using Vec4 = glm::vec4;
using Vec3 = glm::vec3;
using Vec2 = glm::vec2;
using Vec1 = glm::vec1;
using Quat = glm::quat;

Mat4 ViewMatrix(const Vec3 &pos, const Vec3 &up, const Vec3 &forward);

Mat4 OrthographicProjection(f32 left, f32 right, f32 bottom, f32 top, f32 near_plane, f32 far_plane);

Mat4 Mult(const Mat4 m1, const Mat4 m2);

Mat4 TranstationMatrix(const Vec3 &pos);
Mat4 ScaleMatrix(const Vec3 &scale);
Mat4 Inverse(const Mat4 &mat);
Mat4 QuatToMat4(const Quat &q);