#include "Mat2x2.h"

#include <stdexcept>

template <typename T> template <typename U> Mat<T, 2, 2>::Mat(U val) {
  data[0] = static_cast<T>(val);
  data[1] = static_cast<T>(val);
  data[2] = static_cast<T>(val);
  data[3] = static_cast<T>(val);
}

template <typename T>
template <typename A11, typename A12, typename A21, typename A22>
Mat<T, 2, 2>::Mat(A11 a11, A12 a12, A21 a21, A22 a22) {
  data[0] = static_cast<T>(a11);
  data[1] = static_cast<T>(a21);
  data[2] = static_cast<T>(a12);
  data[3] = static_cast<T>(a22);
}

template <typename T>
template <typename U>
Mat<T, 2, 2>::Mat(const Mat<U, 2, 2> &other) {
  data[0] = static_cast<T>(other.data[0]);
  data[1] = static_cast<T>(other.data[1]);
  data[2] = static_cast<T>(other.data[2]);
  data[3] = static_cast<T>(other.data[3]);
}

template <typename T> T &Mat<T, 2, 2>::operator()(u64 r, u64 c) {
  if (r > 1 || r < 0 || c > 1 || c < 0) {
    throw std::invalid_argument("ERROR: Index out of range.");
  }
  return data[r + c * col_size];
}

template <typename T> T Mat<T, 2, 2>::operator()(u64 r, u64 c) const {
  if (r > 1 || r < 0 || c > 1 || c < 0) {
    throw std::invalid_argument("ERROR: Index out of range.");
  }
  return data[r + c * col_size];
}

template <typename T>
template <typename U>
Mat<T, 2, 2> &Mat<T, 2, 2>::operator=(const Mat<U, 2, 2> &other) {
  data[0] = static_cast<T>(other.data[0]);
  data[1] = static_cast<T>(other.data[1]);
  data[2] = static_cast<T>(other.data[2]);
  data[3] = static_cast<T>(other.data[3]);
  return *this;
}

template <typename T>
template <typename U>
Mat<T, 2, 2> &Mat<T, 2, 2>::operator+=(const Mat<U, 2, 2> &other) {
  data[0] += static_cast<T>(other.data[0]);
  data[1] += static_cast<T>(other.data[1]);
  data[2] += static_cast<T>(other.data[2]);
  data[3] += static_cast<T>(other.data[3]);
  return *this;
}

template <typename T>
template <typename U>
Mat<T, 2, 2> &Mat<T, 2, 2>::operator+=(U val) {
  data[0] += static_cast<T>(val);
  data[1] += static_cast<T>(val);
  data[2] += static_cast<T>(val);
  data[3] += static_cast<T>(val);
  return *this;
}

template <typename T>
template <typename U>
Mat<T, 2, 2> &Mat<T, 2, 2>::operator-=(const Mat<U, 2, 2> &other) {
  data[0] -= static_cast<T>(other.data[0]);
  data[1] -= static_cast<T>(other.data[1]);
  data[2] -= static_cast<T>(other.data[2]);
  data[3] -= static_cast<T>(other.data[3]);
  return *this;
}

template <typename T>
template <typename U>
Mat<T, 2, 2> &Mat<T, 2, 2>::operator-=(U val) {
  data[0] -= static_cast<T>(val);
  data[1] -= static_cast<T>(val);
  data[2] -= static_cast<T>(val);
  data[3] -= static_cast<T>(val);
  return *this;
}

template <typename T>
template <typename U>
Mat<T, 2, 2> &Mat<T, 2, 2>::operator*=(U val) {
  data[0] *= static_cast<T>(val);
  data[1] *= static_cast<T>(val);
  data[2] *= static_cast<T>(val);
  data[3] *= static_cast<T>(val);
  return *this;
}

template <typename T>
template <typename U>
Mat<T, 2, 2> &Mat<T, 2, 2>::operator/=(U val) {
  data[0] /= static_cast<T>(val);
  data[1] /= static_cast<T>(val);
  data[2] /= static_cast<T>(val);
  data[3] /= static_cast<T>(val);
  return *this;
}

template <typename T> Mat<T, 2, 2> &Mat<T, 2, 2>::operator++() {
  data[0]++;
  data[1]++;
  data[2]++;
  data[3]++;
  return *this;
}

template <typename T> Mat<T, 2, 2> Mat<T, 2, 2>::operator++(i32) {
  Mat<T, 2, 2> ret(*this);
  ++*this;
  return ret;
}

template <typename T> Mat<T, 2, 2> &Mat<T, 2, 2>::operator--() {
  data[0]--;
  data[1]--;
  data[2]--;
  data[3]--;
  return *this;
}

template <typename T> Mat<T, 2, 2> Mat<T, 2, 2>::operator--(i32) {
  Mat<T, 2, 2> ret(*this);
  --*this;
  return ret;
}

template <typename T, typename U>
Mat<T, 2, 2> operator+(const Mat<T, 2, 2> &m1, const Mat<U, 2, 2> &m2) {
  return Mat<T, 2, 2>{
      m1.a11 + static_cast<T>(m2.a11), m1.a12 + static_cast<T>(m2.a12),
      m1.a21 + static_cast<T>(m2.a21), m1.a22 + static_cast<T>(m2.a22)};
}

template <typename T, typename U>
Mat<T, 2, 2> operator+(const Mat<T, 2, 2> &m, U val) {
  return Mat<T, 2, 2>{m.a11 + static_cast<T>(val), m.a12 + static_cast<T>(val),
                      m.a21 + static_cast<T>(val), m.a22 + static_cast<T>(val)};
}

template <typename T, typename U>
Mat<T, 2, 2> operator+(T val, const Mat<U, 2, 2> &m) {
  return Mat<T, 2, 2>{val + static_cast<T>(m.a11), val + static_cast<T>(m.a12),
                      val + static_cast<T>(m.a21), val + static_cast<T>(m.a22)};
}

template <typename T, typename U>
Mat<T, 2, 2> operator-(const Mat<T, 2, 2> &m1, const Mat<U, 2, 2> &m2) {
  return Mat<T, 2, 2>{
      m1.a11 - static_cast<T>(m2.a11), m1.a12 - static_cast<T>(m2.a12),
      m1.a21 - static_cast<T>(m2.a21), m1.a22 - static_cast<T>(m2.a22)};
}

template <typename T, typename U>
Mat<T, 2, 2> operator-(const Mat<T, 2, 2> &m, U val) {
  return Mat<T, 2, 2>{m.a11 - static_cast<T>(val), m.a12 - static_cast<T>(val),
                      m.a21 - static_cast<T>(val), m.a22 - static_cast<T>(val)};
}

template <typename T, typename U>
Mat<T, 2, 2> operator-(T val, const Mat<U, 2, 2> &m) {
  return Mat<T, 2, 2>{val - static_cast<T>(m.a11), val - static_cast<T>(m.a12),
                      val - static_cast<T>(m.a21), val - static_cast<T>(m.a22)};
}

template <typename T, typename U>
Mat<T, 2, 2> operator*(const Mat<T, 2, 2> &m1, const Mat<U, 2, 2> &m2) {
  return Mat<T, 2, 2>{
      m1.a11 * static_cast<T>(m2.a11) + m1.a12 * static_cast<T>(m2.a21),
      m1.a11 * static_cast<T>(m2.a12) + m1.a12 * static_cast<T>(m2.a22),
      m1.a21 * static_cast<T>(m2.a11) + m1.a22 * static_cast<T>(m2.a21),
      m1.a21 * static_cast<T>(m2.a12) + m1.a22 * static_cast<T>(m2.a22)};
}

template <typename T, typename U>
Mat<T, 2, 2> operator*(const Mat<T, 2, 2> &m, U val) {
  return Mat<T, 2, 2>{m.a11 * static_cast<T>(val), m.a12 * static_cast<T>(val),
                      m.a21 * static_cast<T>(val), m.a22 * static_cast<T>(val)};
}

template <typename T, typename U>
Mat<T, 2, 2> operator*(T val, const Mat<U, 2, 2> &m) {
  return Mat<T, 2, 2>{val * static_cast<T>(m.a11), val * static_cast<T>(m.a12),
                      val * static_cast<T>(m.a21), val * static_cast<T>(m.a22)};
}

template <typename T, typename U>
Mat<T, 2, 2> operator/(const Mat<T, 2, 2> &m, U val) {
  return Mat<T, 2, 2>{m.a11 / static_cast<T>(val), m.a12 / static_cast<T>(val),
                      m.a21 / static_cast<T>(val), m.a22 / static_cast<T>(val)};
}

template <typename T, typename U>
Mat<T, 2, 2> operator/(T val, const Mat<U, 2, 2> &m) {
  return Mat<T, 2, 2>{val / static_cast<T>(m.a11), val / static_cast<T>(m.a12),
                      val / static_cast<T>(m.a21), val / static_cast<T>(m.a22)};
}

template <typename T, typename U>
bool operator==(const Mat<T, 2, 2> &m1, const Mat<U, 2, 2> &m2) {
  return (m1.a11 == static_cast<T>(m2.a11) &&
          m1.a12 == static_cast<T>(m2.a12) &&
          m1.a21 == static_cast<T>(m2.a21) && m1.a22 == static_cast<T>(m2.a22));
}

template <typename T, typename U>
bool operator!=(const Mat<T, 2, 2> &m1, const Mat<U, 2, 2> &m2) {
  return (m1.a11 != static_cast<T>(m2.a11) &&
          m1.a12 != static_cast<T>(m2.a12) &&
          m1.a21 != static_cast<T>(m2.a21) && m1.a22 != static_cast<T>(m2.a22));
}
