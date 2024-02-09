#include "Vec3.h"

#include <stdexcept>

template <typename T>
template <typename U>
inline Vec<T, 3>::Vec(const Vec<U, 3> &other) {
  x = static_cast<T>(other.x);
  y = static_cast<T>(other.y);
  z = static_cast<T>(other.z);
}

template <typename T> template <typename U> inline Vec<T, 3>::Vec(U val) {
  x = static_cast<T>(val);
  y = static_cast<T>(val);
  z = static_cast<T>(val);
}

template <typename T>
template <typename U1, typename U2, typename U3>
inline Vec<T, 3>::Vec(U1 val1, U2 val2, U3 val3) {
  x = static_cast<T>(val1);
  y = static_cast<T>(val2);
  z = static_cast<T>(val3);
}

template <typename T> inline T &Vec<T, 3>::operator()(u32 i) {
  if (i > 2 || i < 0)
    throw std::invalid_argument("ERROR: Subsript out of range");
  return data[i];
}

template <typename T> inline T Vec<T, 3>::operator()(u32 i) const {
  if (i > 2 || i < 0)
    throw std::invalid_argument("ERROR: Subsript out of range");
  return data[i];
}

template <typename T>
template <typename U>
inline Vec<T, 3> &Vec<T, 3>::operator=(const Vec<U, 3> &other) {
  x = static_cast<T>(other.x);
  y = static_cast<T>(other.y);
  z = static_cast<T>(other.z);
  return *this;
}

template <typename T>
template <typename U>
inline Vec<T, 3> &Vec<T, 3>::operator+=(U scalar) {
  x += static_cast<T>(scalar);
  y += static_cast<T>(scalar);
  z += static_cast<T>(scalar);
  return *this;
}

template <typename T>
template <typename U>
inline Vec<T, 3> &Vec<T, 3>::operator+=(const Vec<U, 3> &other) {
  x += static_cast<T>(other.x);
  y += static_cast<T>(other.y);
  z += static_cast<T>(other.z);
  return *this;
}

template <typename T>
template <typename U>
inline Vec<T, 3> &Vec<T, 3>::operator-=(U scalar) {
  x -= static_cast<T>(scalar);
  y -= static_cast<T>(scalar);
  z -= static_cast<T>(scalar);
  return *this;
}

template <typename T>
template <typename U>
inline Vec<T, 3> &Vec<T, 3>::operator-=(const Vec<U, 3> &other) {
  x -= static_cast<T>(other.x);
  y -= static_cast<T>(other.y);
  z -= static_cast<T>(other.z);
  return *this;
}

template <typename T>
template <typename U>
inline Vec<T, 3> &Vec<T, 3>::operator*=(U scalar) {
  x *= static_cast<T>(scalar);
  y *= static_cast<T>(scalar);
  z *= static_cast<T>(scalar);
  return *this;
}

template <typename T>
template <typename U>
inline Vec<T, 3> &Vec<T, 3>::operator*=(const Vec<U, 3> &other) {
  x *= static_cast<T>(other.x);
  y *= static_cast<T>(other.y);
  z *= static_cast<T>(other.z);
  return *this;
}

template <typename T>
template <typename U>
inline Vec<T, 3> &Vec<T, 3>::operator/=(U scalar) {
  x /= static_cast<T>(scalar);
  y /= static_cast<T>(scalar);
  z /= static_cast<T>(scalar);
  return *this;
}

template <typename T>
template <typename U>
inline Vec<T, 3> &Vec<T, 3>::operator/=(const Vec<U, 3> &other) {
  x /= static_cast<T>(other.x);
  y /= static_cast<T>(other.y);
  z /= static_cast<T>(other.z);
  return *this;
}

template <typename T> inline Vec<T, 3> &Vec<T, 3>::operator++() {
  ++x;
  ++y;
  ++z;
  return *this;
}

template <typename T> inline Vec<T, 3> &Vec<T, 3>::operator--() {
  --x;
  --y;
  --z;
  return *this;
}

template <typename T> inline Vec<T, 3> Vec<T, 3>::operator++(i32) {
  Vec<T, 3> result(*this);
  ++*this;
  return result;
}

template <typename T> inline Vec<T, 3> Vec<T, 3>::operator--(i32) {
  Vec<T, 3> result(*this);
  --*this;
  return result;
}

template <typename T, typename U>
inline Vec<T, 3> operator+(const Vec<T, 3> &vec, U scalar) {
  return Vec<T, 3>{vec.x + static_cast<T>(scalar),
                   vec.y + static_cast<T>(scalar),
                   vec.z + static_cast<T>(scalar)};
}

template <typename T, typename U>
inline Vec<T, 3> operator+(const Vec<T, 3> &vec1, const Vec<U, 3> &vec2) {
  return Vec<T, 3>{vec1.x + static_cast<T>(vec2.x),
                   vec1.y + static_cast<T>(vec2.y),
                   vec1.z + static_cast<T>(vec2.z)};
}

template <typename T, typename U>
inline Vec<T, 3> operator+(T scalar, const Vec<U, 3> &vec) {
  return Vec<T, 3>{scalar + static_cast<T>(vec.x),
                   scalar + static_cast<T>(vec.y),
                   scalar + static_cast<T>(vec.z)};
}

template <typename T, typename U>
inline Vec<T, 3> operator-(const Vec<T, 3> &vec, U scalar) {
  return Vec<T, 3>{vec.x - static_cast<T>(scalar),
                   vec.y - static_cast<T>(scalar),
                   vec.z - static_cast<T>(scalar)};
}

template <typename T, typename U>
inline Vec<T, 3> operator-(const Vec<T, 3> &vec1, const Vec<U, 3> &vec2) {
  return Vec<T, 3>{vec1.x - static_cast<T>(vec2.x),
                   vec1.y - static_cast<T>(vec2.y),
                   vec1.z - static_cast<T>(vec2.z)};
}

template <typename T, typename U>
inline Vec<T, 3> operator-(T scalar, const Vec<U, 3> &vec) {
  return Vec<T, 3>{scalar - static_cast<T>(vec.x),
                   scalar - static_cast<T>(vec.y),
                   scalar - static_cast<T>(vec.z)};
}

template <typename T, typename U>
inline Vec<T, 3> operator*(const Vec<T, 3> &vec, U scalar) {
  return Vec<T, 3>{vec.x * static_cast<T>(scalar),
                   vec.y * static_cast<T>(scalar),
                   vec.z * static_cast<T>(scalar)};
}

template <typename T, typename U>
inline Vec<T, 3> operator*(const Vec<T, 3> &vec1, const Vec<U, 3> &vec2) {
  return Vec<T, 3>{vec1.x * static_cast<T>(vec2.x),
                   vec1.y * static_cast<T>(vec2.y),
                   vec1.z * static_cast<T>(vec2.z)};
}

template <typename T, typename U>
inline Vec<T, 3> operator*(T scalar, const Vec<U, 3> &vec) {
  return Vec<T, 3>{scalar * static_cast<T>(vec.x),
                   scalar * static_cast<T>(vec.y),
                   scalar * static_cast<T>(vec.z)};
}

template <typename T, typename U>
inline Vec<T, 3> operator/(const Vec<T, 3> &vec, U scalar) {
  return Vec<T, 3>{vec.x / static_cast<T>(scalar),
                   vec.y / static_cast<T>(scalar),
                   vec.z / static_cast<T>(scalar)};
}

template <typename T, typename U>
inline Vec<T, 3> operator/(const Vec<T, 3> &vec1, const Vec<U, 3> &vec2) {
  return Vec<T, 3>{vec1.x / static_cast<T>(vec2.x),
                   vec1.y / static_cast<T>(vec2.y),
                   vec1.z / static_cast<T>(vec2.z)};
}

template <typename T, typename U>
inline Vec<T, 3> operator/(T scalar, const Vec<U, 3> &vec) {
  return Vec<T, 3>{scalar / static_cast<T>(vec.x),
                   scalar / static_cast<T>(vec.y),
                   scalar / static_cast<T>(vec.z)};
}

template <typename T, typename U>
inline bool operator==(const Vec<T, 3> &vec1, const Vec<U, 3> &vec2) {
  return (vec1.x == vec2.x) && (vec1.y == vec2.y) && (vec1.z == vec2.z);
}

template <typename T, typename U>
inline bool operator!=(const Vec<T, 3> &vec1, const Vec<U, 3> &vec2) {
  return (vec1.x != vec2.x) && (vec1.y != vec2.y) && (vec1.z != vec2.z);
}
