#pragma once

#include "Vec.h"

template <typename T> struct Vec<T, 3> {
  union {
    T data[3];
    struct {
      T x;
      T y;
      T z;
    };
    struct {
      T r;
      T g;
      T b;
    };
    struct {
      T s;
      T t;
      T p;
    };
  };

  static constexpr u64 length = 3;

  template <typename U> Vec(const Vec<U, 3> &other);

  template <typename U> Vec(U val);

  template <typename U1, typename U2, typename U3>
  Vec(U1 val1, U2 val2, U3 val3);

  T &operator()(u32 i);
  T operator()(u32 i) const;

  template <typename U> Vec<T, 3> &operator=(const Vec<U, 3> &other);

  template <typename U> Vec<T, 3> &operator+=(U scalar);

  template <typename U> Vec<T, 3> &operator+=(const Vec<U, 3> &other);

  template <typename U> Vec<T, 3> &operator-=(U scalar);

  template <typename U> Vec<T, 3> &operator-=(const Vec<U, 3> &other);

  template <typename U> Vec<T, 3> &operator*=(U scalar);

  template <typename U> Vec<T, 3> &operator*=(const Vec<U, 3> &other);

  template <typename U> Vec<T, 3> &operator/=(U scalar);

  template <typename U> Vec<T, 3> &operator/=(const Vec<U, 3> &other);

  Vec<T, 3> &operator++();
  Vec<T, 3> &operator--();
  Vec<T, 3> operator++(i32);
  Vec<T, 3> operator--(i32);
};

template <typename T, typename U>
Vec<T, 3> operator+(const Vec<T, 3> &vec, U scalar);

template <typename T, typename U>
Vec<T, 3> operator+(const Vec<T, 3> &vec1, const Vec<U, 3> &vec2);

template <typename T, typename U>
Vec<T, 3> operator+(T scalar, const Vec<U, 3> &vec);

template <typename T, typename U>
Vec<T, 3> operator-(const Vec<T, 3> &vec, U scalar);

template <typename T, typename U>
Vec<T, 3> operator-(const Vec<T, 3> &vec1, const Vec<U, 3> &vec2);

template <typename T, typename U>
Vec<T, 3> operator-(T scalar, const Vec<U, 3> &vec);

template <typename T, typename U>
Vec<T, 3> operator*(const Vec<T, 3> &vec, U scalar);

template <typename T, typename U>
Vec<T, 3> operator*(const Vec<T, 3> &vec1, const Vec<U, 3> &vec2);

template <typename T, typename U>
Vec<T, 3> operator*(T scalar, const Vec<U, 3> &vec);

template <typename T, typename U>
Vec<T, 3> operator/(const Vec<T, 3> &vec, U scalar);

template <typename T, typename U>
Vec<T, 3> operator/(const Vec<T, 3> &vec1, const Vec<U, 3> &vec2);

template <typename T, typename U>
Vec<T, 3> operator/(T scalar, const Vec<U, 3> &vec);

template <typename T, typename U>
bool operator==(const Vec<T, 3> &vec1, const Vec<U, 3> &vec2);

template <typename T, typename U>
bool operator!=(const Vec<T, 3> &vec1, const Vec<U, 3> &vec2);

#include "Vec3.inl"
