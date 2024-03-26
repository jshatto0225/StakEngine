#pragma once

#include "Types.h"

#include <math.h>

#define PI 3.14159265358979323846f

struct vec1 {
  union { f32 x; f32 r; };

  f32 &operator[](i32);
  f32 operator[](i32) const;
};

struct vec2 {
  union {
    f32 x, r;
  };
  union {
    f32 y, g;
  };

  f32 &operator[](i32);
  f32 operator[](i32) const;
};

struct vec3 {
  union {
    f32 x, r;
  };
  union {
    f32 y, g;
  };
  union {
    f32 z, b;
  };

  f32 &operator[](i32);
  f32 operator[](i32) const;
};

struct vec4 {
  union {
    f32 x, r;
  };
  union {
    f32 y, g;
  };
  union {
    f32 z, b;
  };
  union {
    f32 w, a;
  };

  f32 &operator[](i32);
  f32 operator[](i32) const;
};

struct mat2 {
  vec2 cols[2];

  vec2 &operator[](i32);
  const vec2 &operator[](i32) const;
};

struct mat3 {
  vec3 cols[3];

  vec3 &operator[](i32);
  const vec3 &operator[](i32) const;
};

struct mat4 {
  vec4 cols[4];

  vec4 &operator[](i32);
  const vec4 &operator[](i32) const;
};

struct quat {
  f32 x, y, z, w;
};

mat4 ViewMatrix(const vec3 *pos, const vec3 *up, const vec3 *forward);

mat4 OrthographicProjection(f32 Left, f32 Right, f32 Bottom, f32 Top, f32 NearPlane, f32 FarPlane);

mat4 Matmul(const mat4 *Mat1, const mat4 *Mat2);

vec4 MatVecMult(const mat4 *Mat, const vec4 *Vec);

mat4 Matmul(const mat4 *Mat1, const mat4 *Mat2, const mat4 *Mat3);

mat4 TranslationMatrix(const vec3 *Pos);

mat4 ScaleMatrix(const vec3 *Scale);

mat4 Inverse(const mat4 *Mat);

quat DirectionVecToQuat(const vec3 *Vec);

mat4 QuatToMat4(const quat *Quat);

mat4 RotationMatrix2D(f32 Rotation);

f32 Min(f32 A, f32 B);

f32 Max(f32 A, f32 B);
