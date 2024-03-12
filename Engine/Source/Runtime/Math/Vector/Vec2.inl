#include "Vec2.h"

#include <stdexcept>

template <typename T>
inline Vec<T, 2>::Vec() {
  x = 0;
  y = 0;
}

template <typename T>
template <typename U>
inline Vec<T, 2>::Vec(const Vec<U, 2> &other) {
  x = static_cast<T>(other.x);
  y = static_cast<T>(other.y);
}

template <typename T> 
template <typename U> 
inline Vec<T, 2>::Vec(U val) {
  x = static_cast<T>(val);
  y = static_cast<T>(val);
}

template <typename T>
template <typename U1, typename U2>
inline Vec<T, 2>::Vec(U1 val1, U2 val2) {
  x = static_cast<T>(val1);
  y = static_cast<T>(val2);
}

template <typename T> 
inline T &Vec<T, 2>::operator()(u32 i) {
  if (i > 1 || i < 0)
    throw std::invalid_argument("ERROR: Subsript out of range.");
  return data[i];
}

template <typename T> 
inline T Vec<T, 2>::operator()(u32 i) const {
  if (i > 1 || i < 0)
    throw std::invalid_argument("ERROR: Subsript out of range.");
  return data[i];
}

template <typename T>
template <typename U>
inline Vec<T, 2> &Vec<T, 2>::operator=(const Vec<U, 2> &other) {
  x = static_cast<T>(other.x);
  y = static_cast<T>(other.y);
  return *this;
}

template <typename T>
template <typename U>
inline Vec<T, 2> &Vec<T, 2>::operator+=(U scalar) {
  x += static_cast<T>(scalar);
  y += static_cast<T>(scalar);
  return *this;
}

template <typename T>
template <typename U>
inline Vec<T, 2> &Vec<T, 2>::operator+=(const Vec<U, 2> &other) {
  x += static_cast<T>(other.x);
  y += static_cast<T>(other.y);
  return *this;
}

template <typename T>
template <typename U>
inline Vec<T, 2> &Vec<T, 2>::operator-=(U scalar) {
  x -= static_cast<T>(scalar);
  y -= static_cast<T>(scalar);
  return *this;
}

template <typename T>
template <typename U>
inline Vec<T, 2> &Vec<T, 2>::operator-=(const Vec<U, 2> &other) {
  x -= static_cast<T>(other.x);
  y -= static_cast<T>(other.y);
  return *this;
}

template <typename T>
template <typename U>
inline Vec<T, 2> &Vec<T, 2>::operator*=(U scalar) {
  x *= static_cast<T>(scalar);
  y *= static_cast<T>(scalar);
  return *this;
}

template <typename T>
template <typename U>
inline Vec<T, 2> &Vec<T, 2>::operator*=(const Vec<U, 2> &other) {
  x *= static_cast<T>(other.x);
  y *= static_cast<T>(other.y);
  return *this;
}

template <typename T>
template <typename U>
inline Vec<T, 2> &Vec<T, 2>::operator/=(U scalar) {
  x /= static_cast<T>(scalar);
  y /= static_cast<T>(scalar);
  return *this;
}

template <typename T>
template <typename U>
inline Vec<T, 2> &Vec<T, 2>::operator/=(const Vec<U, 2> &other) {
  x /= static_cast<T>(other.x);
  y /= static_cast<T>(other.y);
  return *this;
}

template <typename T> inline Vec<T, 2> &Vec<T, 2>::operator++() {
  ++x;
  ++y;
  return *this;
}

template <typename T> inline Vec<T, 2> &Vec<T, 2>::operator--() {
  --x;
  --y;
  return *this;
}

template <typename T> inline Vec<T, 2> Vec<T, 2>::operator++(i32) {
  Vec<T, 2> result(*this);
  ++*this;
  return result;
}

template <typename T> inline Vec<T, 2> Vec<T, 2>::operator--(i32) {
  Vec<T, 2> result(*this);
  --*this;
  return result;
}

template <typename T, typename U>
inline Vec<T, 2> operator+(const Vec<T, 2> &vec, U scalar) {
  return Vec<T, 2>{vec.x + static_cast<T>(scalar),
                   vec.y + static_cast<T>(scalar)};
}

template <typename T, typename U>
inline Vec<T, 2> operator+(const Vec<T, 2> &vec1, const Vec<U, 2> &vec2) {
  return Vec<T, 2>{vec1.x + static_cast<T>(vec2.x),
                   vec1.y + static_cast<T>(vec2.y)};
}

template <typename T, typename U>
inline Vec<T, 2> operator+(T scalar, const Vec<U, 2> &vec) {
  return Vec<T, 2>{scalar + static_cast<T>(vec.x),
                   scalar + static_cast<T>(vec.y)};
}

template <typename T, typename U>
inline Vec<T, 2> operator-(const Vec<T, 2> &vec, U scalar) {
  return Vec<T, 2>{vec.x - static_cast<T>(scalar),
                   vec.y - static_cast<T>(scalar)};
}

template <typename T, typename U>
inline Vec<T, 2> operator-(const Vec<T, 2> &vec1, const Vec<U, 2> &vec2) {
  return Vec<T, 2>{vec1.x - static_cast<T>(vec2.x),
                   vec1.y - static_cast<T>(vec2.y)};
}

template <typename T, typename U>
inline Vec<T, 2> operator-(T scalar, const Vec<U, 2> &vec) {
  return Vec<T, 2>{scalar - static_cast<T>(vec.x),
                   scalar - static_cast<T>(vec.y)};
}

template <typename T, typename U>
inline Vec<T, 2> operator*(const Vec<T, 2> &vec, U scalar) {
  return Vec<T, 2>{vec.x * static_cast<T>(scalar),
                   vec.y * static_cast<T>(scalar)};
}

template <typename T, typename U>
inline Vec<T, 2> operator*(const Vec<T, 2> &vec1, const Vec<U, 2> &vec2) {
  return Vec<T, 2>{vec1.x * static_cast<T>(vec2.x),
                   vec1.y * static_cast<T>(vec2.y)};
}

template <typename T, typename U>
inline Vec<T, 2> operator*(T scalar, const Vec<U, 2> &vec) {
  return Vec<T, 2>{scalar * static_cast<T>(vec.x),
                   scalar * static_cast<T>(vec.y)};
}

template <typename T, typename U>
inline Vec<T, 2> operator/(const Vec<T, 2> &vec, U scalar) {
  return Vec<T, 2>{vec.x / static_cast<T>(scalar),
                   vec.y / static_cast<T>(scalar)};
}

template <typename T, typename U>
inline Vec<T, 2> operator/(const Vec<T, 2> &vec1, const Vec<U, 2> &vec2) {
  return Vec<T, 2>{vec1.x / static_cast<T>(vec2.x),
                   vec1.y / static_cast<T>(vec2.y)};
}

template <typename T, typename U>
inline Vec<T, 2> operator/(T scalar, const Vec<U, 2> &vec) {
  return Vec<T, 2>{scalar / static_cast<T>(vec.x),
                   scalar / static_cast<T>(vec.y)};
}

template <typename T, typename U>
inline bool operator==(const Vec<T, 2> &vec1, const Vec<U, 2> &vec2) {
  return (vec1.x == vec2.x) && (vec1.y == vec2.y);
}

template <typename T, typename U>
inline bool operator!=(const Vec<T, 2> &vec1, const Vec<U, 2> &vec2) {
  return (vec1.x != vec2.x) && (vec1.y != vec2.y);
}
