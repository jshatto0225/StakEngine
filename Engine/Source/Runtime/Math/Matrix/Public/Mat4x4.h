#pragma once

#include "Mat.h"

template<typename T>
struct Mat<T, 4, 4>
{
    union
    {
        T data[16];
        struct
        {
            T a11;
            T a21;
            T a31;
            T a41;
            T a12;
            T a22;
            T a32;
            T a42;
            T a13;
            T a23;
            T a33;
            T a43;
            T a14;
            T a24;
            T a34;
            T a44;
        };
    };

    static const u64 numRows = 4;
    static const u64 numCols = 4;
    static const u64 rowSize = 4;
    static const u64 colSize = 4;
    static const u64 size = numCols * colSize;

    template<typename U>
    Mat(U val);

    template<typename A11, typename A12, typename A13, typename A14,
             typename A21, typename A22, typename A23, typename A24,
             typename A31, typename A32, typename A33, typename A34,
             typename A41, typename A42, typename A43, typename A44>
    Mat(A11 a11, A12 a12, A13 a13, A14 a14,
        A21 a21, A22 a22, A23 a23, A24 a24,
        A31 a31, A32 a32, A33 a33, A34 a34,
        A41 a41, A42 a42, A43 a43, A44 a44);
    
    template<typename U>
    Mat(const Mat<U, 4, 4>& other);

    T& operator()(u64 r, u64 c);

    template<typename U>
    Mat<T, 4, 4>& operator=(const Mat<U, 4, 4>& other);

    template<typename U>
    Mat<T, 4, 4>& operator+=(const Mat<U, 4, 4>& other);

    template<typename U>
    Mat<T, 4, 4>& operator+=(U val);

    template<typename U>
    Mat<T, 4, 4>& operator-=(const Mat<U, 4, 4>& other);

    template<typename U>
    Mat<T, 4, 4>& operator-=(U val);

    template<typename U>
    Mat<T, 4, 4>& operator*=(U val);

    template<typename U>
    Mat<T, 4, 4>& operator/=(U val);

    Mat<T, 4, 4>& operator++();
    Mat<T, 4, 4>& operator++(i32);
    Mat<T, 4, 4>& operator--();
    Mat<T, 4, 4>& operator--(i32);
};

template<typename T, typename U>
Mat<T, 4, 4> operator+(const Mat<T, 4, 4>& m1, const Mat<U, 4, 4>& m2);

template<typename T, typename U>
Mat<T, 4, 4> operator+(const Mat<T, 4, 4>& m, U val);

template<typename T, typename U>
Mat<T, 4, 4> operator+(T val, const Mat<U, 4, 4>& m);

template<typename T, typename U>
Mat<T, 4, 4> operator-(const Mat<T, 4, 4>& m1, const Mat<U, 4, 4>& m2);

template<typename T, typename U>
Mat<T, 4, 4> operator-(const Mat<T, 4, 4>& m, U val);

template<typename T, typename U>
Mat<T, 4, 4> operator-(T val, const Mat<U, 4, 4>& m);

template<typename T, typename U>
Mat<T, 4, 4> operator*(const Mat<T, 4, 4>& m1, const Mat<U, 4, 4>& m2);

template<typename T, typename U>
Mat<T, 4, 4> operator*(const Mat<T, 4, 4>& m, U val);

template<typename T, typename U>
Mat<T, 4, 4> operator*(T val, const Mat<U, 4, 4>& m);

template<typename T, typename U>
Mat<T, 4, 4> operator/(const Mat<T, 4, 4>& m, U val);

template<typename T, typename U>
Mat<T, 4, 4> operator/(T val, const Mat<U, 4, 4>& m);

template<typename T, typename U>
bool operator==(const Mat<T, 4, 4>& m1, const Mat<U, 4, 4>& m2);

template<typename T, typename U>
bool operator!=(const Mat<T, 4, 4>& m1, const Mat<U, 4, 4>& m2);

#include "Mat4x4.inl"
