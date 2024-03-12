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

/**
 * Create a view matrix
 */
Mat4 ViewMatrix(const Vec3 &pos, const Vec3 &up, const Vec3 &forward);

/**
 * Create an orthographic projection
 */
Mat4 OrthographicProjection(f32 left, f32 right, f32 bottom, f32 top, f32 near_plane, f32 far_plane);

/**
 * matrix multiplication
 */
Mat4 Mult(const Mat4 m1, const Mat4 m2);

/**
 * Produce a translation matrix from a vec3 pos
 */
Mat4 TranstationMatrix(const Vec3 &pos);

/**
 * Produce a scale matrix from a vec3 scale
 */
Mat4 ScaleMatrix(const Vec3 &scale);

/**
 * Invert a matrix
 */
Mat4 Inverse(const Mat4 &mat);

/**
 * Convert a quadternion to a mat4
 */
Mat4 QuatToMat4(const Quat &q);

/**
 * Calculate rotation matrix for 2D object
 * Assumes camera pointing along Z axis
 */
Mat4 RotationMatrix2D(f32 rotation);