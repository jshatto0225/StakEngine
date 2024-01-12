#pragma once

#include "Types.h"

#include "Vec.h"

template<typename T>
struct Vec<T, 1>
{
    union
    {
        T data[1];
        T x;
        T r;
        T s;
    };

    template<typename U>
    Vec(const Vec<U, 1>& other);

    template<typename U>
    Vec(U val);

    T& operator[](u64 i);

    template<typename U>
    Vec<T, 1>& operator=(const Vec<U, 1>& other);

    template<typename U>
    Vec<T, 1>& operator+=(U scalar);

    template<typename U>
    Vec<T, 1>& operator+=(const Vec<U, 1>& other);

    template<typename U>
    Vec<T, 1>& operator-=(U scalar);

    template<typename U>
    Vec<T, 1>& operator-=(const Vec<U, 1>& other);

    template<typename U>
    Vec<T, 1>& operator*=(U scalar);

    template<typename U>
    Vec<T, 1>& operator*=(const Vec<U, 1>& other);

    template<typename U>
    Vec<T, 1>& operator/=(U scalar);

    template<typename U>
    Vec<T, 1>& operator/=(const Vec<U, 1>& other);

    Vec<T, 1>& operator++();
    Vec<T, 1>& operator--();
    Vec<T, 1>& operator++(i32);
    Vec<T, 1>& operator--(i32);
};

template<typename T, typename U>
Vec<T, 1> operator+(const Vec<T, 1>& vec, U scalar);

template<typename T, typename U>
Vec<T, 1> operator+(const Vec<T, 1>& vec1, const Vec<U, 1>& vec2);

template<typename T, typename U>
Vec<T, 1> operator+(T scalar, const Vec<U, 1>& vec);

template<typename T, typename U>
Vec<T, 1> operator-(const Vec<T, 1>& vec, U scalar);

template<typename T, typename U>
Vec<T, 1> operator-(const Vec<T, 1>& vec1, const Vec<U, 1>& vec2);

template<typename T, typename U>
Vec<T, 1> operator-(T scalar, const Vec<U, 1>& vec);

template<typename T, typename U>
Vec<T, 1> operator*(const Vec<T, 1>& vec, U scalar);

template<typename T, typename U>
Vec<T, 1> operator*(const Vec<T, 1>& vec1, const Vec<U, 1>& vec2);

template<typename T, typename U>
Vec<T, 1> operator*(T scalar, const Vec<U, 1>& vec);

template<typename T, typename U>
Vec<T, 1> operator/(const Vec<T, 1>& vec, U scalar);

template<typename T, typename U>
Vec<T, 1> operator/(const Vec<T, 1>& vec1, const Vec<U, 1>& vec2);

template<typename T, typename U>
Vec<T, 1> operator/(T scalar, const Vec<U, 1>& vec);

template<typename T, typename U>
bool operator==(const Vec<T, 1>& vec1, const Vec<U, 1>& vec2);

template<typename T, typename U>
bool operator!=(const Vec<T, 1>& vec1, const Vec<U, 1>& vec2);

#include "Vec1.inl"
