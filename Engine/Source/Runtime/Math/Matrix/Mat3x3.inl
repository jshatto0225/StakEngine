#include "Mat3x3.h"

#include <stdexcept>

template<typename T>
Mat<T, 3, 3>::Mat() {
  data[0] = 0;
  data[1] = 0;
  data[2] = 0;
  data[3] = 0;
  data[4] = 0;
  data[5] = 0;
  data[6] = 0;
  data[7] = 0;
  data[8] = 0;
}

template <typename T> 
template <typename U> 
Mat<T, 3, 3>::Mat(U val) {
  data[0] = static_cast<T>(val);
  data[1] = static_cast<T>(val);
  data[2] = static_cast<T>(val);
  data[3] = static_cast<T>(val);
  data[4] = static_cast<T>(val);
  data[5] = static_cast<T>(val);
  data[6] = static_cast<T>(val);
  data[7] = static_cast<T>(val);
  data[8] = static_cast<T>(val);
}

template <typename T>
template <typename A11, typename A12, typename A13, typename A21, typename A22,
          typename A23, typename A31, typename A32, typename A33>
Mat<T, 3, 3>::Mat(A11 a11, A12 a12, A13 a13, A21 a21, A22 a22, A23 a23, A31 a31,
                  A32 a32, A33 a33) {
  data[0] = static_cast<T>(a11);
  data[1] = static_cast<T>(a21);
  data[2] = static_cast<T>(a31);
  data[3] = static_cast<T>(a12);
  data[4] = static_cast<T>(a22);
  data[5] = static_cast<T>(a32);
  data[6] = static_cast<T>(a13);
  data[7] = static_cast<T>(a23);
  data[8] = static_cast<T>(a33);
}

template <typename T>
template <typename U>
Mat<T, 3, 3>::Mat(const Mat<U, 3, 3> &other) {
  data[0] = static_cast<T>(other.data[0]);
  data[1] = static_cast<T>(other.data[1]);
  data[2] = static_cast<T>(other.data[2]);
  data[3] = static_cast<T>(other.data[3]);
  data[4] = static_cast<T>(other.data[4]);
  data[5] = static_cast<T>(other.data[5]);
  data[6] = static_cast<T>(other.data[6]);
  data[7] = static_cast<T>(other.data[7]);
  data[8] = static_cast<T>(other.data[8]);
}

template <typename T> T &Mat<T, 3, 3>::operator()(u64 r, u64 c) {
  if (r > 2 || r < 0 || c > 2 || c < 0) {
    throw std::invalid_argument("ERROR: Index out of range.");
  }
  return data[r + c * col_size];
}

template <typename T> T Mat<T, 3, 3>::operator()(u64 r, u64 c) const {
  if (r > 2 || r < 0 || c > 2 || c < 0) {
    throw std::invalid_argument("ERROR: Index out of range.");
  }
  return data[r + c * col_size];
}

template <typename T>
template <typename U>
Mat<T, 3, 3> &Mat<T, 3, 3>::operator=(const Mat<U, 3, 3> &other) {
  data[0] = static_cast<T>(other.data[0]);
  data[1] = static_cast<T>(other.data[1]);
  data[2] = static_cast<T>(other.data[2]);
  data[3] = static_cast<T>(other.data[3]);
  data[4] = static_cast<T>(other.data[4]);
  data[5] = static_cast<T>(other.data[5]);
  data[6] = static_cast<T>(other.data[6]);
  data[7] = static_cast<T>(other.data[7]);
  data[8] = static_cast<T>(other.data[8]);
  return *this;
}

template <typename T>
template <typename U>
Mat<T, 3, 3> &Mat<T, 3, 3>::operator+=(const Mat<U, 3, 3> &other) {
  data[0] += static_cast<T>(other.data[0]);
  data[1] += static_cast<T>(other.data[1]);
  data[2] += static_cast<T>(other.data[2]);
  data[3] += static_cast<T>(other.data[3]);
  data[4] += static_cast<T>(other.data[4]);
  data[5] += static_cast<T>(other.data[5]);
  data[6] += static_cast<T>(other.data[6]);
  data[7] += static_cast<T>(other.data[7]);
  data[8] += static_cast<T>(other.data[8]);
  return *this;
}

template <typename T>
template <typename U>
Mat<T, 3, 3> &Mat<T, 3, 3>::operator+=(U val) {
  data[0] += static_cast<T>(val);
  data[1] += static_cast<T>(val);
  data[2] += static_cast<T>(val);
  data[3] += static_cast<T>(val);
  data[4] += static_cast<T>(val);
  data[5] += static_cast<T>(val);
  data[6] += static_cast<T>(val);
  data[7] += static_cast<T>(val);
  data[8] += static_cast<T>(val);
  return *this;
}

template <typename T>
template <typename U>
Mat<T, 3, 3> &Mat<T, 3, 3>::operator-=(const Mat<U, 3, 3> &other) {
  data[0] -= static_cast<T>(other.data[0]);
  data[1] -= static_cast<T>(other.data[1]);
  data[2] -= static_cast<T>(other.data[2]);
  data[3] -= static_cast<T>(other.data[3]);
  data[4] -= static_cast<T>(other.data[4]);
  data[5] -= static_cast<T>(other.data[5]);
  data[6] -= static_cast<T>(other.data[6]);
  data[7] -= static_cast<T>(other.data[7]);
  data[8] -= static_cast<T>(other.data[8]);
  return *this;
}

template <typename T>
template <typename U>
Mat<T, 3, 3> &Mat<T, 3, 3>::operator-=(U val) {
  data[0] -= static_cast<T>(val);
  data[1] -= static_cast<T>(val);
  data[2] -= static_cast<T>(val);
  data[3] -= static_cast<T>(val);
  data[4] -= static_cast<T>(val);
  data[5] -= static_cast<T>(val);
  data[6] -= static_cast<T>(val);
  data[7] -= static_cast<T>(val);
  data[8] -= static_cast<T>(val);
  return *this;
}

template <typename T>
template <typename U>
Mat<T, 3, 3> &Mat<T, 3, 3>::operator*=(U val) {
  data[0] *= static_cast<T>(val);
  data[1] *= static_cast<T>(val);
  data[2] *= static_cast<T>(val);
  data[3] *= static_cast<T>(val);
  data[4] *= static_cast<T>(val);
  data[5] *= static_cast<T>(val);
  data[6] *= static_cast<T>(val);
  data[7] *= static_cast<T>(val);
  data[8] *= static_cast<T>(val);
  return *this;
}

template <typename T>
template <typename U>
Mat<T, 3, 3> &Mat<T, 3, 3>::operator/=(U val) {
  data[0] /= static_cast<T>(val);
  data[1] /= static_cast<T>(val);
  data[2] /= static_cast<T>(val);
  data[3] /= static_cast<T>(val);
  data[4] /= static_cast<T>(val);
  data[5] /= static_cast<T>(val);
  data[6] /= static_cast<T>(val);
  data[7] /= static_cast<T>(val);
  data[8] /= static_cast<T>(val);
  return *this;
}

template <typename T> Mat<T, 3, 3> &Mat<T, 3, 3>::operator++() {
  data[0]++;
  data[1]++;
  data[2]++;
  data[3]++;
  data[4]++;
  data[5]++;
  data[6]++;
  data[7]++;
  data[8]++;
  return *this;
}

template <typename T> Mat<T, 3, 3> Mat<T, 3, 3>::operator++(i32) {
  Mat<T, 3, 3> ret(*this);
  ++*this;
  return ret;
}

template <typename T> Mat<T, 3, 3> &Mat<T, 3, 3>::operator--() {
  data[0]--;
  data[1]--;
  data[2]--;
  data[3]--;
  data[4]--;
  data[5]--;
  data[6]--;
  data[7]--;
  data[8]--;
  return *this;
}

template <typename T> Mat<T, 3, 3> Mat<T, 3, 3>::operator--(i32) {
  Mat<T, 3, 3> ret(*this);
  --*this;
  return ret;
}

template <typename T, typename U>
Mat<T, 3, 3> operator+(const Mat<T, 3, 3> &m1, const Mat<U, 3, 3> &m2) {
  return Mat<T, 3, 3>{
      m1.a11 + static_cast<T>(m2.a11), m1.a12 + static_cast<T>(m2.a12),
      m1.a13 + static_cast<T>(m2.a13), m1.a21 + static_cast<T>(m2.a21),
      m1.a22 + static_cast<T>(m2.a22), m1.a23 + static_cast<T>(m2.a23),
      m1.a31 + static_cast<T>(m2.a31), m1.a32 + static_cast<T>(m2.a32),
      m1.a33 + static_cast<T>(m2.a33)};
}

template <typename T, typename U>
Mat<T, 3, 3> operator+(const Mat<T, 3, 3> &m, U val) {
  return Mat<T, 3, 3>{m.a11 + static_cast<T>(val), m.a12 + static_cast<T>(val),
                      m.a13 + static_cast<T>(val), m.a21 + static_cast<T>(val),
                      m.a22 + static_cast<T>(val), m.a23 + static_cast<T>(val),
                      m.a31 + static_cast<T>(val), m.a32 + static_cast<T>(val),
                      m.a33 + static_cast<T>(val)};
}

template <typename T, typename U>
Mat<T, 3, 3> operator+(T val, const Mat<U, 3, 3> &m) {
  return Mat<T, 3, 3>{val + static_cast<T>(m.a11), val + static_cast<T>(m.a12),
                      val + static_cast<T>(m.a13), val + static_cast<T>(m.a21),
                      val + static_cast<T>(m.a22), val + static_cast<T>(m.a23),
                      val + static_cast<T>(m.a31), val + static_cast<T>(m.a32),
                      val + static_cast<T>(m.a33)};
}

template <typename T, typename U>
Mat<T, 3, 3> operator-(const Mat<T, 3, 3> &m1, const Mat<U, 3, 3> &m2) {
  return Mat<T, 3, 3>{
      m1.a11 - static_cast<T>(m2.a11), m1.a12 - static_cast<T>(m2.a12),
      m1.a13 - static_cast<T>(m2.a13), m1.a21 - static_cast<T>(m2.a21),
      m1.a22 - static_cast<T>(m2.a22), m1.a23 - static_cast<T>(m2.a23),
      m1.a31 - static_cast<T>(m2.a31), m1.a32 - static_cast<T>(m2.a32),
      m1.a33 - static_cast<T>(m2.a33)};
}

template <typename T, typename U>
Mat<T, 3, 3> operator-(const Mat<T, 3, 3> &m, U val) {
  return Mat<T, 3, 3>{m.a11 - static_cast<T>(val), m.a12 - static_cast<T>(val),
                      m.a13 - static_cast<T>(val), m.a21 - static_cast<T>(val),
                      m.a22 - static_cast<T>(val), m.a23 - static_cast<T>(val),
                      m.a31 - static_cast<T>(val), m.a32 - static_cast<T>(val),
                      m.a33 - static_cast<T>(val)};
}

template <typename T, typename U>
Mat<T, 3, 3> operator-(T val, const Mat<U, 3, 3> &m) {
  return Mat<T, 3, 3>{val - static_cast<T>(m.a11), val - static_cast<T>(m.a12),
                      val - static_cast<T>(m.a13), val - static_cast<T>(m.a21),
                      val - static_cast<T>(m.a22), val - static_cast<T>(m.a23),
                      val - static_cast<T>(m.a31), val - static_cast<T>(m.a32),
                      val - static_cast<T>(m.a33)};
}

template <typename T, typename U>
Mat<T, 3, 3> operator*(const Mat<T, 3, 3> &m1, const Mat<U, 3, 3> &m2) {
  return Mat<T, 3, 3>{
      m1.a11 * static_cast<T>(m2.a11) + m1.a12 * static_cast<T>(m2.a21) +
          m1.a13 * static_cast<T>(m2.a31),
      m1.a11 * static_cast<T>(m2.a12) + m1.a12 * static_cast<T>(m2.a22) +
          m1.a13 * static_cast<T>(m2.a32),
      m1.a11 * static_cast<T>(m2.a13) + m1.a12 * static_cast<T>(m2.a23) +
          m1.a13 * static_cast<T>(m2.a33),
      m1.a21 * static_cast<T>(m2.a11) + m1.a22 * static_cast<T>(m2.a21) +
          m1.a23 * static_cast<T>(m2.a31),
      m1.a21 * static_cast<T>(m2.a12) + m1.a22 * static_cast<T>(m2.a22) +
          m1.a23 * static_cast<T>(m2.a32),
      m1.a21 * static_cast<T>(m2.a13) + m1.a22 * static_cast<T>(m2.a23) +
          m1.a23 * static_cast<T>(m2.a33),
      m1.a31 * static_cast<T>(m2.a11) + m1.a32 * static_cast<T>(m2.a21) +
          m1.a33 * static_cast<T>(m2.a31),
      m1.a31 * static_cast<T>(m2.a12) + m1.a32 * static_cast<T>(m2.a22) +
          m1.a33 * static_cast<T>(m2.a32),
      m1.a31 * static_cast<T>(m2.a13) + m1.a32 * static_cast<T>(m2.a23) +
          m1.a33 * static_cast<T>(m2.a33)};
}

template <typename T, typename U>
Mat<T, 3, 3> operator*(const Mat<T, 3, 3> &m, U val) {
  return Mat<T, 3, 3>{m.a11 * static_cast<T>(val), m.a12 * static_cast<T>(val),
                      m.a13 * static_cast<T>(val), m.a21 * static_cast<T>(val),
                      m.a22 * static_cast<T>(val), m.a23 * static_cast<T>(val),
                      m.a31 * static_cast<T>(val), m.a32 * static_cast<T>(val),
                      m.a33 * static_cast<T>(val)};
}

template <typename T, typename U>
Mat<T, 3, 3> operator*(T val, const Mat<U, 3, 3> &m) {
  return Mat<T, 3, 3>{val * static_cast<T>(m.a11), val * static_cast<T>(m.a12),
                      val * static_cast<T>(m.a13), val * static_cast<T>(m.a21),
                      val * static_cast<T>(m.a22), val * static_cast<T>(m.a23),
                      val * static_cast<T>(m.a31), val * static_cast<T>(m.a32),
                      val * static_cast<T>(m.a33)};
}

template <typename T, typename U>
Mat<T, 3, 3> operator/(const Mat<T, 3, 3> &m, U val) {
  return Mat<T, 3, 3>{m.a11 / static_cast<T>(val), m.a12 / static_cast<T>(val),
                      m.a13 / static_cast<T>(val), m.a21 / static_cast<T>(val),
                      m.a22 / static_cast<T>(val), m.a23 / static_cast<T>(val),
                      m.a31 / static_cast<T>(val), m.a32 / static_cast<T>(val),
                      m.a33 / static_cast<T>(val)};
}

template <typename T, typename U>
Mat<T, 3, 3> operator/(T val, const Mat<U, 3, 3> &m) {
  return Mat<T, 3, 3>{val / static_cast<T>(m.a11), val / static_cast<T>(m.a12),
                      val / static_cast<T>(m.a13), val / static_cast<T>(m.a21),
                      val / static_cast<T>(m.a22), val / static_cast<T>(m.a23),
                      val / static_cast<T>(m.a31), val / static_cast<T>(m.a32),
                      val / static_cast<T>(m.a33)};
}

template <typename T, typename U>
bool operator==(const Mat<T, 3, 3> &m1, const Mat<U, 3, 3> &m2) {
  return (
      m1.a11 == static_cast<T>(m2.a11) && m1.a12 == static_cast<T>(m2.a12) &&
      m1.a13 == static_cast<T>(m2.a13) && m1.a21 == static_cast<T>(m2.a21) &&
      m1.a22 == static_cast<T>(m2.a22) && m1.a23 == static_cast<T>(m2.a23) &&
      m1.a31 == static_cast<T>(m2.a31) && m1.a32 == static_cast<T>(m2.a32) &&
      m1.a33 == static_cast<T>(m2.a33));
}

template <typename T, typename U>
bool operator!=(const Mat<T, 3, 3> &m1, const Mat<U, 3, 3> &m2) {
  return (
      m1.a11 != static_cast<T>(m2.a11) && m1.a12 != static_cast<T>(m2.a12) &&
      m1.a13 != static_cast<T>(m2.a13) && m1.a21 != static_cast<T>(m2.a21) &&
      m1.a22 != static_cast<T>(m2.a22) && m1.a23 != static_cast<T>(m2.a23) &&
      m1.a31 != static_cast<T>(m2.a31) && m1.a32 != static_cast<T>(m2.a32) &&
      m1.a33 != static_cast<T>(m2.a33));
}
