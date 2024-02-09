#pragma once

#include "Mat.h"

template <typename T> struct Mat<T, 3, 3> {
  union {
    T data[9];
    struct {
      T a11;
      T a21;
      T a31;
      T a12;
      T a22;
      T a32;
      T a13;
      T a23;
      T a33;
    };
  };

  static const u64 num_rows = 3;
  static const u64 num_cols = 3;
  static const u64 row_size = 3;
  static const u64 col_size = 3;
  static const u64 size = num_cols * col_size;

  template <typename U> Mat(U val);

  template <typename A11, typename A12, typename A13, typename A21,
            typename A22, typename A23, typename A31, typename A32,
            typename A33>
  Mat(A11 a11, A12 a12, A13 a13, A21 a21, A22 a22, A23 a23, A31 a31, A32 a32,
      A33 a33);

  template <typename U> Mat(const Mat<U, 3, 3> &other);

  T &operator()(u64 r, u64 c);
  T operator()(u64 r, u64 c) const;

  template <typename U> Mat<T, 3, 3> &operator=(const Mat<U, 3, 3> &other);

  template <typename U> Mat<T, 3, 3> &operator+=(const Mat<U, 3, 3> &other);

  template <typename U> Mat<T, 3, 3> &operator+=(U val);

  template <typename U> Mat<T, 3, 3> &operator-=(const Mat<U, 3, 3> &other);

  template <typename U> Mat<T, 3, 3> &operator-=(U val);

  template <typename U> Mat<T, 3, 3> &operator*=(U val);

  template <typename U> Mat<T, 3, 3> &operator/=(U val);

  Mat<T, 3, 3> &operator++();
  Mat<T, 3, 3> operator++(i32);
  Mat<T, 3, 3> &operator--();
  Mat<T, 3, 3> operator--(i32);
};

template <typename T, typename U>
Mat<T, 3, 3> operator+(const Mat<T, 3, 3> &m1, const Mat<U, 3, 3> &m2);

template <typename T, typename U>
Mat<T, 3, 3> operator+(const Mat<T, 3, 3> &m, U val);

template <typename T, typename U>
Mat<T, 3, 3> operator+(T val, const Mat<U, 3, 3> &m);

template <typename T, typename U>
Mat<T, 3, 3> operator-(const Mat<T, 3, 3> &m1, const Mat<U, 3, 3> &m2);

template <typename T, typename U>
Mat<T, 3, 3> operator-(const Mat<T, 3, 3> &m, U val);

template <typename T, typename U>
Mat<T, 3, 3> operator-(T val, const Mat<U, 3, 3> &m);

template <typename T, typename U>
Mat<T, 3, 3> operator*(const Mat<T, 3, 3> &m1, const Mat<U, 3, 3> &m2);

template <typename T, typename U>
Mat<T, 3, 3> operator*(const Mat<T, 3, 3> &m, U val);

template <typename T, typename U>
Mat<T, 3, 3> operator*(T val, const Mat<U, 3, 3> &m);

template <typename T, typename U>
Mat<T, 3, 3> operator/(const Mat<T, 3, 3> &m, U val);

template <typename T, typename U>
Mat<T, 3, 3> operator/(T val, const Mat<U, 3, 3> &m);

template <typename T, typename U>
bool operator==(const Mat<T, 3, 3> &m1, const Mat<U, 3, 3> &m2);

template <typename T, typename U>
bool operator!=(const Mat<T, 3, 3> &m1, const Mat<U, 3, 3> &m2);

#include "Mat3x3.inl"
