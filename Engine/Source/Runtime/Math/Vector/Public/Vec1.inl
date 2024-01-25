#pragma once

#include "Vec1.h"

#include <stdexcept>

template<typename T>
template<typename U>
inline Vec<T, 1>::Vec(const Vec<U, 1>& other)
{
    x = static_cast<T>(other.x);
}

template<typename T>
template<typename U>
inline Vec<T, 1>::Vec(U val)
{
    x = static_cast<T>(val);
}

template<typename T>
inline T& Vec<T, 1>::operator()(u32 i)
{
    if(i != 0)
        throw std::invalid_argument("ERROR: Subsript out of range");
    return data[i];
}

template<typename T>
inline T Vec<T, 1>::operator()(u32 i) const
{
    if(i != 0)
        throw std::invalid_argument("ERROR: Subsript out of range");
    return data[i];
}

template<typename T>
template<typename U>
inline Vec<T, 1>& Vec<T, 1>::operator=(const Vec<U, 1>& other)
{
    x = static_cast<T>(other.x);
    return *this;
}

template<typename T>
template<typename U>
inline Vec<T, 1>& Vec<T, 1>::operator+=(U scalar)
{
    x += static_cast<T>(scalar);
    return *this;
}

template<typename T>
template<typename U>
inline Vec<T, 1>& Vec<T, 1>::operator+=(const Vec<U, 1>& other)
{
    x += static_cast<T>(other.x);
    return *this;
}

template<typename T>
template<typename U>
inline Vec<T, 1>& Vec<T, 1>::operator-=(U scalar)
{
    x -= static_cast<T>(scalar);
    return *this;
}

template<typename T>
template<typename U>
inline Vec<T, 1>& Vec<T, 1>::operator-=(const Vec<U, 1>& other)
{
    x -= static_cast<T>(other.x);
    return *this;
}

template<typename T>
template<typename U>
inline Vec<T, 1>& Vec<T, 1>::operator*=(U scalar)
{
    x *= static_cast<T>(scalar);
    return *this;
}

template<typename T>
template<typename U>
inline Vec<T, 1>& Vec<T, 1>::operator*=(const Vec<U, 1>& other)
{
    x *= static_cast<T>(other.x);
    return *this;
}

template<typename T>
template<typename U>
inline Vec<T, 1>& Vec<T, 1>::operator/=(U scalar)
{
    x /= static_cast<T>(scalar);
    return *this;
}

template<typename T>
template<typename U>
inline Vec<T, 1>& Vec<T, 1>::operator/=(const Vec<U, 1>& other)
{
    x /= static_cast<T>(other.x);
    return *this;
}

template<typename T>
inline Vec<T, 1>& Vec<T, 1>::operator++()
{
    ++x;
    return *this;
}

template<typename T>
inline Vec<T, 1>& Vec<T, 1>::operator--()
{
    --x;
    return *this;
}

template<typename T>
inline Vec<T, 1> Vec<T, 1>::operator++(i32)
{
    Vec<T, 1> result(*this);
    ++*this;
    return result;
}

template<typename T>
inline Vec<T, 1> Vec<T, 1>::operator--(i32)
{
    Vec<T, 1> result(*this);
    --*this;
    return result;
}

template<typename T, typename U>
inline Vec<T, 1> operator+(const Vec<T, 1>& vec, U scalar)
{
    return Vec<T, 1>{ vec.x + static_cast<T>(scalar) };
}

template<typename T, typename U>
inline Vec<T, 1> operator+(const Vec<T, 1>& vec1, const Vec<U, 1>& vec2)
{
    return Vec<T, 1>{ vec1.x + static_cast<T>(vec2.x) };
}

template<typename T, typename U>
inline Vec<T, 1> operator+(T scalar, const Vec<U, 1>& vec)
{
    return Vec<T, 1>{ scalar + static_cast<T>(vec.x) };
}

template<typename T, typename U>
inline Vec<T, 1> operator-(const Vec<T, 1>& vec, U scalar)
{
    return Vec<T, 1>{ vec.x - static_cast<T>(scalar) };
}

template<typename T, typename U>
inline Vec<T, 1> operator-(const Vec<T, 1>& vec1, const Vec<U, 1>& vec2)
{
    return Vec<T, 1>{ vec1.x - static_cast<T>(vec2.x) };
}

template<typename T, typename U>
inline Vec<T, 1> operator-(T scalar, const Vec<U, 1>& vec)
{
    return Vec<T, 1>{ scalar - static_cast<T>(vec.x) };
}

template<typename T, typename U>
inline Vec<T, 1> operator*(const Vec<T, 1>& vec, U scalar)
{
    return Vec<T, 1>{ vec.x * static_cast<T>(scalar) };
}

template<typename T, typename U>
inline Vec<T, 1> operator*(const Vec<T, 1>& vec1, const Vec<U, 1>& vec2)
{
    return Vec<T, 1>{ vec1.x * static_cast<T>(vec2.x) };
}

template<typename T, typename U>
inline Vec<T, 1> operator*(T scalar, const Vec<U, 1>& vec)
{
    return Vec<T, 1>{ scalar * static_cast<T>(vec.x) };
}

template<typename T, typename U>
inline Vec<T, 1> operator/(const Vec<T, 1>& vec, U scalar)
{
    return Vec<T, 1>{ vec.x / static_cast<T>(scalar) };
}

template<typename T, typename U>
inline Vec<T, 1> operator/(const Vec<T, 1>& vec1, const Vec<U, 1>& vec2)
{
    return Vec<T, 1>{ vec1.x / static_cast<T>(vec2.x)};
}

template<typename T, typename U>
inline Vec<T, 1> operator/(T scalar, const Vec<U, 1>& vec)
{
    return Vec<T, 1>{ scalar / static_cast<T>(vec.x) };
}

template<typename T, typename U>
inline bool operator==(const Vec<T, 1>& vec1, const Vec<U, 1>& vec2)
{
    return vec1.x == vec2.x;
}

template<typename T, typename U>
inline bool operator!=(const Vec<T, 1>& vec1, const Vec<U, 1>& vec2)
{
    return vec1.x != vec2.x;
}
