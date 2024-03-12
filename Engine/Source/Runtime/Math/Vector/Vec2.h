#pragma once

#include "Vec.h"

template <typename T> struct Vec<T, 2> {
  union {
    T data[2];
    struct {
      T x;
      T y;
    };
    struct {
      T r;
      T g;
    };
    struct {
      T s;
      T t;
    };
  };

  static constexpr u64 length = 2;

  Vec();

  template <typename U> 
  Vec(const Vec<U, 2> &other);

  template <typename U> 
  Vec(U val = 0);

  template <typename U1, typename U2> Vec(U1 val1, U2 val2);

  T &operator()(u32 i);
  T operator()(u32 i) const;

  template <typename U> Vec<T, 2> &operator=(const Vec<U, 2> &other);

  template <typename U> Vec<T, 2> &operator+=(U scalar);

  template <typename U> Vec<T, 2> &operator+=(const Vec<U, 2> &other);

  template <typename U> Vec<T, 2> &operator-=(U scalar);

  template <typename U> Vec<T, 2> &operator-=(const Vec<U, 2> &other);

  template <typename U> Vec<T, 2> &operator*=(U scalar);

  template <typename U> Vec<T, 2> &operator*=(const Vec<U, 2> &other);

  template <typename U> Vec<T, 2> &operator/=(U scalar);

  template <typename U> Vec<T, 2> &operator/=(const Vec<U, 2> &other);

  Vec<T, 2> &operator++();
  Vec<T, 2> &operator--();
  Vec<T, 2> operator++(i32);
  Vec<T, 2> operator--(i32);
};

template <typename T, typename U>
Vec<T, 2> operator+(const Vec<T, 2> &vec, U scalar);

template <typename T, typename U>
Vec<T, 2> operator+(const Vec<T, 2> &vec1, const Vec<U, 2> &vec2);

template <typename T, typename U>
Vec<T, 2> operator+(T scalar, const Vec<U, 2> &vec);

template <typename T, typename U>
Vec<T, 2> operator-(const Vec<T, 2> &vec, U scalar);

template <typename T, typename U>
Vec<T, 2> operator-(const Vec<T, 2> &vec1, const Vec<U, 2> &vec2);

template <typename T, typename U>
Vec<T, 2> operator-(T scalar, const Vec<U, 2> &vec);

template <typename T, typename U>
Vec<T, 2> operator*(const Vec<T, 2> &vec, U scalar);

template <typename T, typename U>
Vec<T, 2> operator*(const Vec<T, 2> &vec1, const Vec<U, 2> &vec2);

template <typename T, typename U>
Vec<T, 2> operator*(T scalar, const Vec<U, 2> &vec);

template <typename T, typename U>
Vec<T, 2> operator/(const Vec<T, 2> &vec, U scalar);

template <typename T, typename U>
Vec<T, 2> operator/(const Vec<T, 2> &vec1, const Vec<U, 2> &vec2);

template <typename T, typename U>
Vec<T, 2> operator/(T scalar, const Vec<U, 2> &vec);

template <typename T, typename U>
bool operator==(const Vec<T, 2> &vec1, const Vec<U, 2> &vec2);

template <typename T, typename U>
bool operator!=(const Vec<T, 2> &vec1, const Vec<U, 2> &vec2);

#include "Vec2.inl"
