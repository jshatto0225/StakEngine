#pragma once

#include "Vec.h"

template<typename T>
T Dot(const Vec<T, 3> &v1, const Vec<T, 3> &v2);

template<typename T>
Vec<T, 3> Normalize(const Vec<T, 3> &v);

template<typename T>
Vec<T, 3> Cross(const Vec<T, 3> &v1, const Vec<T, 3> &v2);

template<typename T>
Mat<T, 4, 4> OrthographicProjection(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far);

#include "Operations.inl"
