#include "StakMath.h"

f32 &vec1::operator[](i32 i) {
  return x;
}

f32 vec1::operator[](i32 i) const {
  return x;
}

f32 &vec2::operator[](i32 i) {
  if (i == 1) {
    return y;
  }
  else {
    return x;
  }
}

f32 vec2::operator[](i32 i) const {
  if (i == 1) {
    return y;
  }
  else {
    return x;
  }
}

f32 &vec3::operator[](i32 i) {
  if (i == 2) {
    return z;
  }
  else if (i == 1) {
    return y;
  }
  else {
    return x;
  }
}

f32 vec3::operator[](i32 i) const {
  if (i == 2) {
    return z;
  }
  else if (i == 1) {
    return y;
  }
  else {
    return x;
  }
}

f32 &vec4::operator[](i32 i) {
  if (i == 3) {
    return w;
  }
  else if (i == 2) {
    return z;
  }
  else if (i == 1) {
    return y;
  }
  else {
    return x;
  }
}

f32 vec4::operator[](i32 i) const {
  if (i == 3) {
    return w;
  }
  else if (i == 2) {
    return z;
  }
  else if (i == 1) {
    return y;
  }
  else {
    return x;
  }
}

vec2 &mat2::operator[](i32 i) {
  return cols[i % 2];
}

const vec2 &mat2::operator[](i32 i) const {
  return cols[i % 2];
}

vec3 &mat3::operator[](i32 i) {
  return cols[i % 3];
}

const vec3 &mat3::operator[](i32 i) const {
  return cols[i % 3];
}

vec4 &mat4::operator[](i32 i) {
  return cols[i % 4];
}

const vec4 &mat4::operator[](i32 i) const {
  return cols[i % 4];
}

mat4 ViewMatrix(const vec3 &Pos, const vec3 &Up, const vec3 &Forward) {
  // TODO: Finish
  mat4 Result = {};
  Result[0][0] = 1.0f;
  Result[1][1] = 1.0f;
  Result[2][2] = 1.0f;
  Result[3][3] = 1.0f;
  return Result;
}

mat4 OrthographicProjection(f32 Left, f32 Right, f32 Bottom, f32 Top, f32 NearPlane, f32 FarPlane) {
  mat4 Result = {};
  Result[0][0] = 2 / (Right - Left);
  Result[1][1] = 2 / (Top - Bottom);
  Result[2][2] = 2 / (FarPlane - NearPlane);
  Result[3][3] = 1.0f;
  Result[3][0] = -(Right + Left) / (Right - Left);
  Result[3][1] = -(Top + Bottom) / (Top - Bottom);
  Result[3][2] = -(FarPlane + NearPlane) / (FarPlane - NearPlane);
  
  return Result;
}

mat4 Matmul(const mat4 *Mat1, const mat4 *Mat2) {
  return {
    (*Mat1)[0][0] * (*Mat2)[0][0] + (*Mat1)[1][0] * (*Mat2)[0][1] + (*Mat1)[2][0] * (*Mat2)[0][2] + (*Mat1)[3][0] * (*Mat2)[0][3],
    (*Mat1)[0][1] * (*Mat2)[0][0] + (*Mat1)[1][1] * (*Mat2)[0][1] + (*Mat1)[2][1] * (*Mat2)[0][2] + (*Mat1)[3][1] * (*Mat2)[0][3],
    (*Mat1)[0][2] * (*Mat2)[0][0] + (*Mat1)[1][2] * (*Mat2)[0][1] + (*Mat1)[2][2] * (*Mat2)[0][2] + (*Mat1)[3][2] * (*Mat2)[0][3],
    (*Mat1)[0][3] * (*Mat2)[0][0] + (*Mat1)[1][3] * (*Mat2)[0][1] + (*Mat1)[2][3] * (*Mat2)[0][2] + (*Mat1)[3][3] * (*Mat2)[0][3],

    (*Mat1)[0][0] * (*Mat2)[1][0] + (*Mat1)[1][0] * (*Mat2)[1][1] + (*Mat1)[2][0] * (*Mat2)[1][2] + (*Mat1)[3][0] * (*Mat2)[1][3],
    (*Mat1)[0][1] * (*Mat2)[1][0] + (*Mat1)[1][1] * (*Mat2)[1][1] + (*Mat1)[2][1] * (*Mat2)[1][2] + (*Mat1)[3][1] * (*Mat2)[1][3],
    (*Mat1)[0][2] * (*Mat2)[1][0] + (*Mat1)[1][2] * (*Mat2)[1][1] + (*Mat1)[2][2] * (*Mat2)[1][2] + (*Mat1)[3][2] * (*Mat2)[1][3],
    (*Mat1)[0][3] * (*Mat2)[1][0] + (*Mat1)[1][3] * (*Mat2)[1][1] + (*Mat1)[2][3] * (*Mat2)[1][2] + (*Mat1)[3][3] * (*Mat2)[1][3],

    (*Mat1)[0][0] * (*Mat2)[2][0] + (*Mat1)[1][0] * (*Mat2)[2][1] + (*Mat1)[2][0] * (*Mat2)[2][2] + (*Mat1)[3][0] * (*Mat2)[2][3],
    (*Mat1)[0][1] * (*Mat2)[2][0] + (*Mat1)[1][1] * (*Mat2)[2][1] + (*Mat1)[2][1] * (*Mat2)[2][2] + (*Mat1)[3][1] * (*Mat2)[2][3],
    (*Mat1)[0][2] * (*Mat2)[2][0] + (*Mat1)[1][2] * (*Mat2)[2][1] + (*Mat1)[2][2] * (*Mat2)[2][2] + (*Mat1)[3][2] * (*Mat2)[2][3],
    (*Mat1)[0][3] * (*Mat2)[2][0] + (*Mat1)[1][3] * (*Mat2)[2][1] + (*Mat1)[2][3] * (*Mat2)[2][2] + (*Mat1)[3][3] * (*Mat2)[2][3],

    (*Mat1)[0][0] * (*Mat2)[3][0] + (*Mat1)[1][0] * (*Mat2)[3][1] + (*Mat1)[2][0] * (*Mat2)[3][2] + (*Mat1)[3][0] * (*Mat2)[3][3],
    (*Mat1)[0][1] * (*Mat2)[3][0] + (*Mat1)[1][1] * (*Mat2)[3][1] + (*Mat1)[2][1] * (*Mat2)[3][2] + (*Mat1)[3][1] * (*Mat2)[3][3],
    (*Mat1)[0][2] * (*Mat2)[3][0] + (*Mat1)[1][2] * (*Mat2)[3][1] + (*Mat1)[2][2] * (*Mat2)[3][2] + (*Mat1)[3][2] * (*Mat2)[3][3],
    (*Mat1)[0][3] * (*Mat2)[3][0] + (*Mat1)[1][3] * (*Mat2)[3][1] + (*Mat1)[2][3] * (*Mat2)[3][2] + (*Mat1)[3][3] * (*Mat2)[3][3]
  };
}

vec4 MatVecMult(const mat4 *Mat, const vec4 *Vec) {
  return  {
    (*Mat)[0][0] * (*Vec)[0] + (*Mat)[1][0] * (*Vec)[1] + (*Mat)[2][0] * (*Vec)[2] + (*Mat)[3][0] * (*Vec)[3],
    (*Mat)[0][1] * (*Vec)[0] + (*Mat)[1][1] * (*Vec)[1] + (*Mat)[2][1] * (*Vec)[2] + (*Mat)[3][1] * (*Vec)[3],
    (*Mat)[0][2] * (*Vec)[0] + (*Mat)[1][2] * (*Vec)[1] + (*Mat)[2][2] * (*Vec)[2] + (*Mat)[3][2] * (*Vec)[3],
    (*Mat)[0][3] * (*Vec)[0] + (*Mat)[1][3] * (*Vec)[1] + (*Mat)[2][3] * (*Vec)[2] + (*Mat)[3][3] * (*Vec)[3]
  };
}

mat4 Matmul(const mat4 *Mat1, const mat4 *Mat2, const mat4 *Mat3) {
  mat4 Result = {};

  Result = Matmul(Mat1, Mat2);
  Result = Matmul(&Result, Mat3);

  return Result;
}

mat4 TranslationMatrix(const vec3 *Pos) {
  mat4 Result = {};

  Result[0][0] = 1;
  Result[1][1] = 1;
  Result[2][2] = 1;
  Result[3][3] = 1;
  Result[3][0] = Pos->x;
  Result[3][1] = Pos->y;
  Result[3][2] = Pos->z;

  return Result;
}

quat DirectionVecToQuat(const vec3 *Vec) {
  quat Result = {};

  f32 CosX = cos(Vec->x * 0.5);
  f32 CosY = cos(Vec->y * 0.5);
  f32 CosZ = cos(Vec->z * 0.5);

  f32 SinX = sin(Vec->x * 0.5);
  f32 SinY = sin(Vec->y * 0.5);
  f32 SinZ = sin(Vec->z * 0.5);

  Result.w = CosX * CosY * CosZ + SinX * SinY * SinZ;
  Result.x = SinX * CosY * CosZ - CosX * SinY * SinZ;
  Result.y = -(CosX * SinY * CosZ + SinX * CosY * SinZ);
  Result.z = -(CosX * CosY * SinZ - SinX * SinY * CosZ);

  return Result;
}

mat4 Inverse(const mat4 *Mat) {
  mat4 Result = {};
 
  f32 A2323 = (*Mat)[2][2] * (*Mat)[3][3] - (*Mat)[2][3] * (*Mat)[3][2];
  f32 A1323 = (*Mat)[2][1] * (*Mat)[3][3] - (*Mat)[2][3] * (*Mat)[3][1];
  f32 A1223 = (*Mat)[2][1] * (*Mat)[3][2] - (*Mat)[2][2] * (*Mat)[3][1];
  f32 A0323 = (*Mat)[2][0] * (*Mat)[3][3] - (*Mat)[2][3] * (*Mat)[3][0];
  f32 A0223 = (*Mat)[2][0] * (*Mat)[3][2] - (*Mat)[2][2] * (*Mat)[3][0];
  f32 A0123 = (*Mat)[2][0] * (*Mat)[3][1] - (*Mat)[2][1] * (*Mat)[3][0];
  f32 A2313 = (*Mat)[1][2] * (*Mat)[3][3] - (*Mat)[1][3] * (*Mat)[3][2];
  f32 A1313 = (*Mat)[1][1] * (*Mat)[3][3] - (*Mat)[1][3] * (*Mat)[3][1];
  f32 A1213 = (*Mat)[1][1] * (*Mat)[3][2] - (*Mat)[1][2] * (*Mat)[3][1];
  f32 A2312 = (*Mat)[1][2] * (*Mat)[2][3] - (*Mat)[1][3] * (*Mat)[2][2];
  f32 A1312 = (*Mat)[1][1] * (*Mat)[2][3] - (*Mat)[1][3] * (*Mat)[2][1];
  f32 A1212 = (*Mat)[1][1] * (*Mat)[2][2] - (*Mat)[1][2] * (*Mat)[2][1];
  f32 A0313 = (*Mat)[1][0] * (*Mat)[3][3] - (*Mat)[1][3] * (*Mat)[3][0];
  f32 A0213 = (*Mat)[1][0] * (*Mat)[3][2] - (*Mat)[1][2] * (*Mat)[3][0];
  f32 A0312 = (*Mat)[1][0] * (*Mat)[2][3] - (*Mat)[1][3] * (*Mat)[2][0];
  f32 A0212 = (*Mat)[1][0] * (*Mat)[2][2] - (*Mat)[1][2] * (*Mat)[2][0];
  f32 A0113 = (*Mat)[1][0] * (*Mat)[3][1] - (*Mat)[1][1] * (*Mat)[3][0];
  f32 A0112 = (*Mat)[1][0] * (*Mat)[2][1] - (*Mat)[1][1] * (*Mat)[2][0];

  f32 Det = (*Mat)[0][0] * ( (*Mat)[1][1] * A2323 - (*Mat)[1][2] * A1323 + (*Mat)[1][3] * A1223 ) 
          - (*Mat)[0][1] * ( (*Mat)[1][0] * A2323 - (*Mat)[1][2] * A0323 + (*Mat)[1][3] * A0223 ) 
          + (*Mat)[0][2] * ( (*Mat)[1][0] * A1323 - (*Mat)[1][1] * A0323 + (*Mat)[1][3] * A0123 ) 
          - (*Mat)[0][3] * ( (*Mat)[1][0] * A1223 - (*Mat)[1][1] * A0223 + (*Mat)[1][2] * A0123 );
  Det = 1 / Det;

   Result[0][0] = Det *   ( (*Mat)[1][1] * A2323 - (*Mat)[1][2] * A1323 + (*Mat)[1][3] * A1223 );
   Result[0][1] = Det * - ( (*Mat)[0][1] * A2323 - (*Mat)[0][2] * A1323 + (*Mat)[0][3] * A1223 );
   Result[0][2] = Det *   ( (*Mat)[0][1] * A2313 - (*Mat)[0][2] * A1313 + (*Mat)[0][3] * A1213 );
   Result[0][3] = Det * - ( (*Mat)[0][1] * A2312 - (*Mat)[0][2] * A1312 + (*Mat)[0][3] * A1212 );
   Result[1][0] = Det * - ( (*Mat)[1][0] * A2323 - (*Mat)[1][2] * A0323 + (*Mat)[1][3] * A0223 );
   Result[1][1] = Det *   ( (*Mat)[0][0] * A2323 - (*Mat)[0][2] * A0323 + (*Mat)[0][3] * A0223 );
   Result[1][2] = Det * - ( (*Mat)[0][0] * A2313 - (*Mat)[0][2] * A0313 + (*Mat)[0][3] * A0213 );
   Result[1][3] = Det *   ( (*Mat)[0][0] * A2312 - (*Mat)[0][2] * A0312 + (*Mat)[0][3] * A0212 );
   Result[2][0] = Det *   ( (*Mat)[1][0] * A1323 - (*Mat)[1][1] * A0323 + (*Mat)[1][3] * A0123 );
   Result[2][1] = Det * - ( (*Mat)[0][0] * A1323 - (*Mat)[0][1] * A0323 + (*Mat)[0][3] * A0123 );
   Result[2][2] = Det *   ( (*Mat)[0][0] * A1313 - (*Mat)[0][1] * A0313 + (*Mat)[0][3] * A0113 );
   Result[2][3] = Det * - ( (*Mat)[0][0] * A1312 - (*Mat)[0][1] * A0312 + (*Mat)[0][3] * A0112 );
   Result[3][0] = Det * - ( (*Mat)[1][0] * A1223 - (*Mat)[1][1] * A0223 + (*Mat)[1][2] * A0123 );
   Result[3][1] = Det *   ( (*Mat)[0][0] * A1223 - (*Mat)[0][1] * A0223 + (*Mat)[0][2] * A0123 );
   Result[3][2] = Det * - ( (*Mat)[0][0] * A1213 - (*Mat)[0][1] * A0213 + (*Mat)[0][2] * A0113 );
   Result[3][3] = Det *   ( (*Mat)[0][0] * A1212 - (*Mat)[0][1] * A0212 + (*Mat)[0][2] * A0112 );

  return Result;
}

mat4 QuatToMat4(const quat *Quat) {
  mat4 Mat = { 1, 0, 0, 0,
               0, 1, 0, 0,
               0, 0, 1, 0,
               0, 0, 0, 1 };

  f32 QXX = Quat->x * Quat->x;
  f32 QYY = Quat->y * Quat->y;
  f32 QZZ = Quat->z * Quat->z;
  f32 QXZ = Quat->x * Quat->z;
  f32 QXY = Quat->x * Quat->y;
  f32 QYZ = Quat->y * Quat->z;
  f32 QWX = Quat->w * Quat->x;
  f32 QWY = Quat->w * Quat->y;
  f32 QWZ = Quat->w * Quat->z;

  Mat[0][0] = 1 - 2 * (QYY + QZZ);
  Mat[0][1] = 2 * (QXY + QWZ);
  Mat[0][2] = 2 * (QXZ - QWY);

  Mat[1][0] = 2 * (QXY - QWZ);
  Mat[1][1] = 1 - 2 * (QXX + QZZ);
  Mat[1][2] = 2 * (QYZ + QWX);

  Mat[2][0] = 2 * (QXZ + QWY);
  Mat[2][1] = 2 * (QYZ - QWX);
  Mat[2][2] = 1 - 2 * (QXX + QYY);

  return Mat;
}

mat4 ScaleMatrix(const vec3 *Scale) {
  mat4 Result = {};
  Result[0][0] = Scale->x;
  Result[1][1] = Scale->y;
  Result[2][2] = Scale->z;
  Result[3][3] = 1.0f;
  return Result;
}

mat4 RotationMatrix2D(f32 Rotation) {
  f32 Angle = Rotation;
  f32 Cos = cos(Angle);
  f32 Sin = sin(Angle);

  mat4 Rotate = {};

  Rotate[0][0] = Cos;
  Rotate[0][1] = -Sin;
  Rotate[1][0] = Sin;
  Rotate[1][1] = Cos;
  Rotate[2][2] = 1;
  Rotate[3][3] = 1;

  return Rotate;
}

f32 Min(f32 A, f32 B) {
  return A < B ? A : B;
}

f32 Max(f32 A, f32 B) {
  return A > B ? A : B;
}