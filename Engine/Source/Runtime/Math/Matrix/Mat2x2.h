#pragma once

#include "Mat.h"

template <typename T> struct Mat<T, 2, 2> {
  union {
    T data[4];
    struct {
      T a11;
      T a21;
      T a12;
      T a22;
    };
  };

  static const u64 num_rows = 2;
  static const u64 num_cols = 2;
  static const u64 row_size = 2;
  static const u64 col_size = 2;
  static const u64 size = num_cols * col_size;

  Mat();

  template <typename U> 
  Mat(U val);

  template <typename A11, typename A12, typename A21, typename A22>
  Mat(A11 a11, A12 a12, A21 a21, A22 a22);

  template <typename U> 
  Mat(const Mat<U, 2, 2> &other);

  T &operator()(u64 r, u64 c);
  T operator()(u64 r, u64 c) const;

  template <typename U> Mat<T, 2, 2> &operator=(const Mat<U, 2, 2> &other);

  template <typename U> Mat<T, 2, 2> &operator+=(const Mat<U, 2, 2> &other);

  template <typename U> Mat<T, 2, 2> &operator+=(U val);

  template <typename U> Mat<T, 2, 2> &operator-=(const Mat<U, 2, 2> &other);

  template <typename U> Mat<T, 2, 2> &operator-=(U val);

  template <typename U> Mat<T, 2, 2> &operator*=(U val);

  template <typename U> Mat<T, 2, 2> &operator/=(U val);

  Mat<T, 2, 2> &operator++();
  Mat<T, 2, 2> operator++(i32);
  Mat<T, 2, 2> &operator--();
  Mat<T, 2, 2> operator--(i32);
};

template <typename T, typename U>
Mat<T, 2, 2> operator+(const Mat<T, 2, 2> &m1, const Mat<U, 2, 2> &m2);

template <typename T, typename U>
Mat<T, 2, 2> operator+(const Mat<T, 2, 2> &m, U val);

template <typename T, typename U>
Mat<T, 2, 2> operator+(T val, const Mat<U, 2, 2> &m);

template <typename T, typename U>
Mat<T, 2, 2> operator-(const Mat<T, 2, 2> &m1, const Mat<U, 2, 2> &m2);

template <typename T, typename U>
Mat<T, 2, 2> operator-(const Mat<T, 2, 2> &m, U val);

template <typename T, typename U>
Mat<T, 2, 2> operator-(T val, const Mat<U, 2, 2> &m);

template <typename T, typename U>
Mat<T, 2, 2> operator*(const Mat<T, 2, 2> &m1, const Mat<U, 2, 2> &m2);

template <typename T, typename U>
Mat<T, 2, 2> operator*(const Mat<T, 2, 2> &m, U val);

template <typename T, typename U>
Mat<T, 2, 2> operator*(T val, const Mat<U, 2, 2> &m);

template <typename T, typename U>
Mat<T, 2, 2> operator/(const Mat<T, 2, 2> &m, U val);

template <typename T, typename U>
Mat<T, 2, 2> operator/(T val, const Mat<U, 2, 2> &m);

template <typename T, typename U>
bool operator==(const Mat<T, 2, 2> &m1, const Mat<U, 2, 2> &m2);

template <typename T, typename U>
bool operator!=(const Mat<T, 2, 2> &m1, const Mat<U, 2, 2> &m2);

#include "Mat2x2.inl"
