#pragma once

#include "Types.h"

#include "Vec.h"

template<typename T>
struct Vec<T, 4>
{
    union
    {
        T data[4];
        struct
        {
            T x;
            T y;
            T z;
            T w;
        };
        struct
        {
            T r;
            T g;
            T b;
            T a;
        };
        struct
        {
            T s;
            T t;
            T p;
            T q;
        };
    };

    template<typename U>
    Vec(const Vec<U, 4>& other);

    template<typename U>
    Vec(U val);

    template<typename U1, typename U2, typename U3, typename U4>
    Vec(U1 val1, U2 val2, U3 val3, U4 val4);

    T& operator[](u64 i);

    template<typename U>
    Vec<T, 4>& operator=(const Vec<U, 4>& other);

    template<typename U>
    Vec<T, 4>& operator+=(U scalar);

    template<typename U>
    Vec<T, 4>& operator+=(const Vec<U, 4>& other);

    template<typename U>
    Vec<T, 4>& operator-=(U scalar);

    template<typename U>
    Vec<T, 4>& operator-=(const Vec<U, 4>& other);

    template<typename U>
    Vec<T, 4>& operator*=(U scalar);

    template<typename U>
    Vec<T, 4>& operator*=(const Vec<U, 4>& other);

    template<typename U>
    Vec<T, 4>& operator/=(U scalar);

    template<typename U>
    Vec<T, 4>& operator/=(const Vec<U, 4>& other);

    Vec<T, 4>& operator++();
    Vec<T, 4>& operator--();
    Vec<T, 4>& operator++(i32);
    Vec<T, 4>& operator--(i32);
};

template<typename T, typename U>
Vec<T, 4> operator+(const Vec<T, 4>& vec, U scalar);

template<typename T, typename U>
Vec<T, 4> operator+(const Vec<T, 4>& vec1, const Vec<U, 4>& vec2);

template<typename T, typename U>
Vec<T, 4> operator+(T scalar, const Vec<U, 4>& vec);

template<typename T, typename U>
Vec<T, 4> operator-(const Vec<T, 4>& vec, U scalar);

template<typename T, typename U>
Vec<T, 4> operator-(const Vec<T, 4>& vec1, const Vec<U, 4>& vec2);

template<typename T, typename U>
Vec<T, 4> operator-(T scalar, const Vec<U, 4>& vec);

template<typename T, typename U>
Vec<T, 4> operator*(const Vec<T, 4>& vec, U scalar);

template<typename T, typename U>
Vec<T, 4> operator*(const Vec<T, 4>& vec1, const Vec<U, 4>& vec2);

template<typename T, typename U>
Vec<T, 4> operator*(T scalar, const Vec<U, 4>& vec);

template<typename T, typename U>
Vec<T, 4> operator/(const Vec<T, 4>& vec, U scalar);

template<typename T, typename U>
Vec<T, 4> operator/(const Vec<T, 4>& vec1, const Vec<U, 4>& vec2);

template<typename T, typename U>
Vec<T, 4> operator/(T scalar, const Vec<U, 4>& vec);

template<typename T, typename U>
bool operator==(const Vec<T, 4>& vec1, const Vec<U, 4>& vec2);

template<typename T, typename U>
bool operator!=(const Vec<T, 4>& vec1, const Vec<U, 4>& vec2);

#include "Vec4.inl"
