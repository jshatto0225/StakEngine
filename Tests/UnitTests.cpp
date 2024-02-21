#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#define SK_NO_MAIN
#include "Runtime/Runtime.h"

TEST_CASE("Test Vec1", "[Vec1]")
{
    Vec1 v = { 5 };
    Vec1 v2 = { 2 };
    REQUIRE(v.x == 5);
    v += 2;
    REQUIRE(v.x == 7);
    v -= 2;
    REQUIRE(v.x == 5);
    v *= 2;
    REQUIRE(v.x == 10);
    v /= 2;
    REQUIRE(v.x == 5);
    v++;
    REQUIRE(v.x == 6);
    v--;
    REQUIRE(v.x == 5);
    --v;
    REQUIRE(v.x == 4);
    ++v;
    REQUIRE(v.x == 5);

    v += v2;
    REQUIRE(v.x == 7);
    v -= v2;
    REQUIRE(v.x == 5);
    v *= v2;
    REQUIRE(v.x == 10);
    v /= v2;
    REQUIRE(v.x == 5);

    REQUIRE((v + 2).x == 7);
    REQUIRE((2 + v).x == 7);
    REQUIRE((v + v2).x == 7);
    REQUIRE((v - 2).x == 3);
    REQUIRE((2 - v).x == -3);
    REQUIRE((v - v2).x == 3);
    REQUIRE((v * 2).x == 10);
    REQUIRE((2 * v).x == 10);
    REQUIRE((v * v2).x == 10);
    REQUIRE((v / 2).x == (5.0f / 2.0f));
    REQUIRE((2 / v).x == (2 / 5));
    REQUIRE((v / v2).x == (5.0f / 2.0f));
    REQUIRE_FALSE(v == v2);
    REQUIRE(v != v2);

    REQUIRE(v.x == v.r);
    REQUIRE(v.x == v.s);
    REQUIRE(v.x == v.data[0]);
    REQUIRE(v.x == v(0));
    REQUIRE_THROWS(v(1));
} // End Test Vec1

TEST_CASE("Test Vec2", "[Vec2]")
{
    Vec2 v = { 5, 2 };
    Vec2 v2 = { 2, 5 };
    REQUIRE(v.x == 5);
    REQUIRE(v.y == 2);
    v += 2;
    REQUIRE(v.x == 7);
    REQUIRE(v.y == 4);
    v -= 2;
    REQUIRE(v.x == 5);
    REQUIRE(v.y == 2);
    v *= 2;
    REQUIRE(v.x == 10);
    REQUIRE(v.y == 4);
    v /= 2;
    REQUIRE(v.x == 5);
    REQUIRE(v.y == 2);
    v++;
    REQUIRE(v.x == 6);
    REQUIRE(v.y == 3);
    v--;
    REQUIRE(v.x == 5);
    REQUIRE(v.y == 2);
    --v;
    REQUIRE(v.x == 4);
    REQUIRE(v.y == 1);
    ++v;
    REQUIRE(v.x == 5);
    REQUIRE(v.y == 2);

    v += v2;
    REQUIRE(v.x == 7);
    REQUIRE(v.y == 7);
    v -= v2;
    REQUIRE(v.x == 5);
    REQUIRE(v.y == 2);
    v *= v2;
    REQUIRE(v.x == 10);
    REQUIRE(v.y == 10);
    v /= v2;
    REQUIRE(v.x == 5);
    REQUIRE(v.y == 2);

    REQUIRE((v + 2).x == 7);
    REQUIRE((v + 2).y == 4);
    REQUIRE((2 + v).x == 7);
    REQUIRE((2 + v).y == 4);
    REQUIRE((v + v2).x == 7);
    REQUIRE((v + v2).y == 7);
    REQUIRE((v - 2).x == 3);
    REQUIRE((v - 2).y == 0);
    REQUIRE((2 - v).x == -3);
    REQUIRE((2-v).y == 0);
    REQUIRE((v - v2).x == 3);
    REQUIRE((v - v2).y == -3);
    REQUIRE((v * 2).x == 10);
    REQUIRE((v * 2).y == 4);
    REQUIRE((2 * v).x == 10);
    REQUIRE((2 * v).y == 4);
    REQUIRE((v * v2).x == 10);
    REQUIRE((v * v2).y == 10);
    REQUIRE((v / 2).x == (5.0f / 2.0f));
    REQUIRE((v / 2).y == (2.0f / 2.0f));
    REQUIRE((2 / v).x == (2 / 5));
    REQUIRE((2 / v).y == (2 / 2));
    REQUIRE((v / v2).x == (5.0f / 2.0f));
    REQUIRE((v / v2).y == (2.0f / 5.0f));
    REQUIRE_FALSE(v == v2);
    REQUIRE(v != v2);

    REQUIRE(v.x == v.r);
    REQUIRE(v.x == v.s);
    REQUIRE(v.x == v.data[0]);
    REQUIRE(v.x == v(0));

    REQUIRE(v.y == v.g);
    REQUIRE(v.y == v.t);
    REQUIRE(v.y == v.data[1]);
    REQUIRE(v.y == v(1));

    REQUIRE_THROWS(v(2));
} // End Test Vec2

TEST_CASE("Test Vec3", "[Vec3]")
{
    Vec3 v = { 5, 2, 5 };
    Vec3 v2 = { 2, 5, 2 };
    REQUIRE(v.x == 5);
    REQUIRE(v.y == 2);
    REQUIRE(v.z == 5);
    v += 2;
    REQUIRE(v.x == 7);
    REQUIRE(v.y == 4);
    REQUIRE(v.z == 7);
    v -= 2;
    REQUIRE(v.x == 5);
    REQUIRE(v.y == 2);
    REQUIRE(v.z == 5);
    v *= 2;
    REQUIRE(v.x == 10);
    REQUIRE(v.y == 4);
    REQUIRE(v.z == 10);
    v /= 2;
    REQUIRE(v.x == 5);
    REQUIRE(v.y == 2);
    REQUIRE(v.z == 5);
    v++;
    REQUIRE(v.x == 6);
    REQUIRE(v.y == 3);
    REQUIRE(v.z == 6);
    v--;
    REQUIRE(v.x == 5);
    REQUIRE(v.y == 2);
    REQUIRE(v.z == 5);
    --v;
    REQUIRE(v.x == 4);
    REQUIRE(v.y == 1);
    REQUIRE(v.z == 4);
    ++v;
    REQUIRE(v.x == 5);
    REQUIRE(v.y == 2);
    REQUIRE(v.z == 5);

    v += v2;
    REQUIRE(v.x == 7);
    REQUIRE(v.y == 7);
    REQUIRE(v.z == 7);
    v -= v2;
    REQUIRE(v.x == 5);
    REQUIRE(v.y == 2);
    REQUIRE(v.z == 5);
    v *= v2;
    REQUIRE(v.x == 10);
    REQUIRE(v.y == 10);
    REQUIRE(v.z == 10);
    v /= v2;
    REQUIRE(v.x == 5);
    REQUIRE(v.y == 2);
    REQUIRE(v.z == 5);

    REQUIRE((v + 2).x == 7);
    REQUIRE((v + 2).y == 4);
    REQUIRE((v + 2).z == 7);

    REQUIRE((2 + v).x == 7);
    REQUIRE((2 + v).y == 4);
    REQUIRE((2 + v).z == 7);

    REQUIRE((v + v2).x == 7);
    REQUIRE((v + v2).y == 7);
    REQUIRE((v + v2).z == 7);

    REQUIRE((v - 2).x == 3);
    REQUIRE((v - 2).y == 0);
    REQUIRE((v - 2).z == 3);

    REQUIRE((2 - v).x == -3);
    REQUIRE((2 - v).y == 0);
    REQUIRE((2 - v).z == -3);

    REQUIRE((v - v2).x == 3);
    REQUIRE((v - v2).y == -3);
    REQUIRE((v - v2).z == 3);

    REQUIRE((v * 2).x == 10);
    REQUIRE((v * 2).y == 4);
    REQUIRE((v * 2).z == 10);

    REQUIRE((2 * v).x == 10);
    REQUIRE((2 * v).y == 4);
    REQUIRE((2 * v).z == 10);

    REQUIRE((v * v2).x == 10);
    REQUIRE((v * v2).y == 10);
    REQUIRE((v * v2).z == 10);

    REQUIRE((v / 2).x == (5.0f / 2.0f));
    REQUIRE((v / 2).y == (2.0f / 2.0f));
    REQUIRE((v / 2).z == (5.0f / 2.0f));

    REQUIRE((2 / v).x == (2 / 5));
    REQUIRE((2 / v).y == (2 / 2));
    REQUIRE((2 / v).z == (2 / 5));

    REQUIRE((v / v2).x == (5.0f / 2.0f));
    REQUIRE((v / v2).y == (2.0f / 5.0f));
    REQUIRE((v / v2).z == (5.0f / 2.0f));

    REQUIRE_FALSE(v == v2);
    REQUIRE(v != v2);

    REQUIRE(v.x == v.r);
    REQUIRE(v.x == v.s);
    REQUIRE(v.x == v.data[0]);
    REQUIRE(v.x == v(0));

    REQUIRE(v.y == v.g);
    REQUIRE(v.y == v.t);
    REQUIRE(v.y == v.data[1]);
    REQUIRE(v.y == v(1));

    REQUIRE(v.z == v.b);
    REQUIRE(v.z == v.p);
    REQUIRE(v.z == v.data[2]);
    REQUIRE(v.z == v(2));

    REQUIRE_THROWS(v(3));
} // End Test Vec3

TEST_CASE("Test Vec4", "[Vec4]")
{
    Vec4 v = { 5, 2, 5, 2 };
    Vec4 v2 = { 2, 5, 2, 5 };
    REQUIRE(v.x == 5);
    REQUIRE(v.y == 2);
    REQUIRE(v.z == 5);
    REQUIRE(v.w == 2);
    v += 2;
    REQUIRE(v.x == 7);
    REQUIRE(v.y == 4);
    REQUIRE(v.z == 7);
    REQUIRE(v.w == 4);
    v -= 2;
    REQUIRE(v.x == 5);
    REQUIRE(v.y == 2);
    REQUIRE(v.z == 5);
    REQUIRE(v.w == 2);
    v *= 2;
    REQUIRE(v.x == 10);
    REQUIRE(v.y == 4);
    REQUIRE(v.z == 10);
    REQUIRE(v.w == 4);
    v /= 2;
    REQUIRE(v.x == 5);
    REQUIRE(v.y == 2);
    REQUIRE(v.z == 5);
    REQUIRE(v.w == 2);
    v++;
    REQUIRE(v.x == 6);
    REQUIRE(v.y == 3);
    REQUIRE(v.z == 6);
    REQUIRE(v.w == 3);
    v--;
    REQUIRE(v.x == 5);
    REQUIRE(v.y == 2);
    REQUIRE(v.z == 5);
    REQUIRE(v.w == 2);
    --v;
    REQUIRE(v.x == 4);
    REQUIRE(v.y == 1);
    REQUIRE(v.z == 4);
    REQUIRE(v.w == 1);
    ++v;
    REQUIRE(v.x == 5);
    REQUIRE(v.y == 2);
    REQUIRE(v.z == 5);
    REQUIRE(v.w == 2);

    v += v2;
    REQUIRE(v.x == 7);
    REQUIRE(v.y == 7);
    REQUIRE(v.z == 7);
    REQUIRE(v.w == 7);
    v -= v2;
    REQUIRE(v.x == 5);
    REQUIRE(v.y == 2);
    REQUIRE(v.z == 5);
    REQUIRE(v.w == 2);
    v *= v2;
    REQUIRE(v.x == 10);
    REQUIRE(v.y == 10);
    REQUIRE(v.z == 10);
    REQUIRE(v.w == 10);
    v /= v2;
    REQUIRE(v.x == 5);
    REQUIRE(v.y == 2);
    REQUIRE(v.z == 5);
    REQUIRE(v.w == 2);

    REQUIRE((v + 2).x == 7);
    REQUIRE((v + 2).y == 4);
    REQUIRE((v + 2).z == 7);
    REQUIRE((v + 2).w == 4);

    REQUIRE((2 + v).x == 7);
    REQUIRE((2 + v).y == 4);
    REQUIRE((2 + v).z == 7);
    REQUIRE((2 + v).w == 4);

    REQUIRE((v + v2).x == 7);
    REQUIRE((v + v2).y == 7);
    REQUIRE((v + v2).z == 7);
    REQUIRE((v + v2).w == 7);

    REQUIRE((v - 2).x == 3);
    REQUIRE((v - 2).y == 0);
    REQUIRE((v - 2).z == 3);
    REQUIRE((v - 2).w == 0);

    REQUIRE((2 - v).x == -3);
    REQUIRE((2 - v).y == 0);
    REQUIRE((2 - v).z == -3);
    REQUIRE((2 - v).w == 0);

    REQUIRE((v - v2).x == 3);
    REQUIRE((v - v2).y == -3);
    REQUIRE((v - v2).z == 3);
    REQUIRE((v - v2).w == -3);

    REQUIRE((v * 2).x == 10);
    REQUIRE((v * 2).y == 4);
    REQUIRE((v * 2).z == 10);
    REQUIRE((v * 2).w == 4);

    REQUIRE((2 * v).x == 10);
    REQUIRE((2 * v).y == 4);
    REQUIRE((2 * v).z == 10);
    REQUIRE((2 * v).w == 4);

    REQUIRE((v * v2).x == 10);
    REQUIRE((v * v2).y == 10);
    REQUIRE((v * v2).z == 10);
    REQUIRE((v * v2).w == 10);

    REQUIRE((v / 2).x == (5.0f / 2.0f));
    REQUIRE((v / 2).y == (2.0f / 2.0f));
    REQUIRE((v / 2).z == (5.0f / 2.0f));
    REQUIRE((v / 2).w == (2.0f / 2.0f));

    REQUIRE((2 / v).x == (2 / 5));
    REQUIRE((2 / v).y == (2 / 2));
    REQUIRE((2 / v).z == (2 / 5));
    REQUIRE((2 / v).w == (2 / 2));

    REQUIRE((v / v2).x == (5.0f / 2.0f));
    REQUIRE((v / v2).y == (2.0f / 5.0f));
    REQUIRE((v / v2).z == (5.0f / 2.0f));
    REQUIRE((v / v2).w == (2.0f / 5.0f));

    REQUIRE_FALSE(v == v2);
    REQUIRE(v != v2);

    REQUIRE(v.x == v.r);
    REQUIRE(v.x == v.s);
    REQUIRE(v.x == v.data[0]);
    REQUIRE(v.x == v(0));

    REQUIRE(v.y == v.g);
    REQUIRE(v.y == v.t);
    REQUIRE(v.y == v.data[1]);
    REQUIRE(v.y == v(1));

    REQUIRE(v.z == v.b);
    REQUIRE(v.z == v.p);
    REQUIRE(v.z == v.data[2]);
    REQUIRE(v.z == v(2));

    REQUIRE(v.w == v.a);
    REQUIRE(v.w == v.q);
    REQUIRE(v.w == v.data[3]);
    REQUIRE(v.w == v(3));

    REQUIRE_THROWS(v(4));
} // End Test Vec4

TEST_CASE("Test Mat2x2", "[Mat2x2]")
{
    Mat2 m(1, 2,
           3, 4);
    REQUIRE(m.a11 == 1);
    REQUIRE(m.a12 == 2);
    REQUIRE(m.a21 == 3);
    REQUIRE(m.a22 == 4);
    REQUIRE(m(0, 0) == m.a11);
    REQUIRE(m(0, 1) == m.a12);
    REQUIRE(m(1, 0) == m.a21);
    REQUIRE(m(1, 1) == m.a22);
    REQUIRE_THROWS(m(3, 1));
    REQUIRE_THROWS(m(1, 3));

    m += 1;
    REQUIRE(m(0, 0) == 2);
    REQUIRE(m(0, 1) == 3);
    REQUIRE(m(1, 0) == 4);
    REQUIRE(m(1, 1) == 5);

    m -= 1;
    REQUIRE(m(0, 0) == 1);
    REQUIRE(m(0, 1) == 2);
    REQUIRE(m(1, 0) == 3);
    REQUIRE(m(1, 1) == 4);

    m *= 2;
    REQUIRE(m(0, 0) == 2);
    REQUIRE(m(0, 1) == 4);
    REQUIRE(m(1, 0) == 6);
    REQUIRE(m(1, 1) == 8);

    m /= 2;
    REQUIRE(m(0, 0) == 1);
    REQUIRE(m(0, 1) == 2);
    REQUIRE(m(1, 0) == 3);
    REQUIRE(m(1, 1) == 4);

    m++;
    REQUIRE(m(0, 0) == 2);
    REQUIRE(m(0, 1) == 3);
    REQUIRE(m(1, 0) == 4);
    REQUIRE(m(1, 1) == 5);

    m--;
    REQUIRE(m(0, 0) == 1);
    REQUIRE(m(0, 1) == 2);
    REQUIRE(m(1, 0) == 3);
    REQUIRE(m(1, 1) == 4);

    ++m;
    REQUIRE(m(0, 0) == 2);
    REQUIRE(m(0, 1) == 3);
    REQUIRE(m(1, 0) == 4);
    REQUIRE(m(1, 1) == 5);

    --m;
    REQUIRE(m(0, 0) == 1);
    REQUIRE(m(0, 1) == 2);
    REQUIRE(m(1, 0) == 3);
    REQUIRE(m(1, 1) == 4);

    m(0, 0) = 100;
    REQUIRE(m(0, 0) == 100);
    m(0, 0) = 1;

    Mat2 m2(m);
    REQUIRE(m2(0, 0) == 1);
    REQUIRE(m2(0, 1) == 2);
    REQUIRE(m2(1, 0) == 3);
    REQUIRE(m2(1, 1) == 4);

    REQUIRE(m == m2);
    REQUIRE_FALSE(m != m2);

    REQUIRE((m + m2)(0, 0) == 2);
    REQUIRE((m + m2)(0, 1) == 4);
    REQUIRE((m + m2)(1, 0) == 6);
    REQUIRE((m + m2)(1, 1) == 8);

    REQUIRE((m + 1)(0, 0) == 2);
    REQUIRE((m + 1)(0, 1) == 3);
    REQUIRE((m + 1)(1, 0) == 4);
    REQUIRE((m + 1)(1, 1) == 5);

    REQUIRE((1 + m)(0, 0) == 2);
    REQUIRE((1 + m)(0, 1) == 3);
    REQUIRE((1 + m)(1, 0) == 4);
    REQUIRE((1 + m)(1, 1) == 5);

    REQUIRE((m - m2)(0, 0) == 0);
    REQUIRE((m - m2)(0, 1) == 0);
    REQUIRE((m - m2)(1, 0) == 0);
    REQUIRE((m - m2)(1, 1) == 0);

    REQUIRE((m - 1)(0, 0) == 0);
    REQUIRE((m - 2)(0, 1) == 0);
    REQUIRE((m - 3)(1, 0) == 0);
    REQUIRE((m - 4)(1, 1) == 0);

    REQUIRE((1 - m)(0, 0) == 0);
    REQUIRE((2 - m)(0, 1) == 0);
    REQUIRE((3 - m)(1, 0) == 0);
    REQUIRE((4 - m)(1, 1) == 0);

    REQUIRE((m * m2)(0, 0) == 7);
    REQUIRE((m * m2)(0, 1) == 10);
    REQUIRE((m * m2)(1, 0) == 15);
    REQUIRE((m * m2)(1, 1) == 22);

    REQUIRE((m * 2)(0, 0) == 2);
    REQUIRE((m * 2)(0, 1) == 4);
    REQUIRE((m * 2)(1, 0) == 6);
    REQUIRE((m * 2)(1, 1) == 8);

    REQUIRE((2 * m)(0, 0) == 2);
    REQUIRE((2 * m)(0, 1) == 4);
    REQUIRE((2 * m)(1, 0) == 6);
    REQUIRE((2 * m)(1, 1) == 8);

    REQUIRE((m / 2)(0, 0) == 0.5);
    REQUIRE((m / 2)(0, 1) == 1);
    REQUIRE((m / 2)(1, 0) == 1.5);
    REQUIRE((m / 2)(1, 1) == 2);

    REQUIRE((2 / m)(0, 0) == 2);
    REQUIRE((2 / m)(0, 1) == 1);
    REQUIRE((2 / m)(1, 0) == 0);
    REQUIRE((2 / m)(1, 1) == 0);
} // End Test Mat2x2

TEST_CASE("Test Mat3x3", "[Mat3x3]")
{
    Mat3 m(1, 2, 3,
           4, 5, 6,
           7, 8, 9);
    REQUIRE(m.a11 == 1);
    REQUIRE(m.a12 == 2);
    REQUIRE(m.a13 == 3);
    REQUIRE(m.a21 == 4);
    REQUIRE(m.a22 == 5);
    REQUIRE(m.a23 == 6);
    REQUIRE(m.a31 == 7);
    REQUIRE(m.a32 == 8);
    REQUIRE(m.a33 == 9);
    REQUIRE(m(0, 0) == m.a11);
    REQUIRE(m(0, 1) == m.a12);
    REQUIRE(m(0, 2) == m.a13);
    REQUIRE(m(1, 0) == m.a21);
    REQUIRE(m(1, 1) == m.a22);
    REQUIRE(m(1, 2) == m.a23);
    REQUIRE(m(2, 0) == m.a31);
    REQUIRE(m(2, 1) == m.a32);
    REQUIRE(m(2, 2) == m.a33);
    REQUIRE_THROWS(m(3, 1));
    REQUIRE_THROWS(m(1, 3));

    m += 1;
    REQUIRE(m(0, 0) == 2);
    REQUIRE(m(0, 1) == 3);
    REQUIRE(m(0, 2) == 4);
    REQUIRE(m(1, 0) == 5);
    REQUIRE(m(1, 1) == 6);
    REQUIRE(m(1, 2) == 7);
    REQUIRE(m(2, 0) == 8);
    REQUIRE(m(2, 1) == 9);
    REQUIRE(m(2, 2) == 10);

    m -= 1;
    REQUIRE(m(0, 0) == 1);
    REQUIRE(m(0, 1) == 2);
    REQUIRE(m(0, 2) == 3);
    REQUIRE(m(1, 0) == 4);
    REQUIRE(m(1, 1) == 5);
    REQUIRE(m(1, 2) == 6);
    REQUIRE(m(2, 0) == 7);
    REQUIRE(m(2, 1) == 8);
    REQUIRE(m(2, 2) == 9);

    m *= 2;
    REQUIRE(m(0, 0) == 2);
    REQUIRE(m(0, 1) == 4);
    REQUIRE(m(0, 2) == 6);
    REQUIRE(m(1, 0) == 8);
    REQUIRE(m(1, 1) == 10);
    REQUIRE(m(1, 2) == 12);
    REQUIRE(m(2, 0) == 14);
    REQUIRE(m(2, 1) == 16);
    REQUIRE(m(2, 2) == 18);

    m /= 2;
    REQUIRE(m(0, 0) == 1);
    REQUIRE(m(0, 1) == 2);
    REQUIRE(m(0, 2) == 3);
    REQUIRE(m(1, 0) == 4);
    REQUIRE(m(1, 1) == 5);
    REQUIRE(m(1, 2) == 6);
    REQUIRE(m(2, 0) == 7);
    REQUIRE(m(2, 1) == 8);
    REQUIRE(m(2, 2) == 9);

    m++;
    REQUIRE(m(0, 0) == 2);
    REQUIRE(m(0, 1) == 3);
    REQUIRE(m(0, 2) == 4);
    REQUIRE(m(1, 0) == 5);
    REQUIRE(m(1, 1) == 6);
    REQUIRE(m(1, 2) == 7);
    REQUIRE(m(2, 0) == 8);
    REQUIRE(m(2, 1) == 9);
    REQUIRE(m(2, 2) == 10);

    m--;
    REQUIRE(m(0, 0) == 1);
    REQUIRE(m(0, 1) == 2);
    REQUIRE(m(0, 2) == 3);
    REQUIRE(m(1, 0) == 4);
    REQUIRE(m(1, 1) == 5);
    REQUIRE(m(1, 2) == 6);
    REQUIRE(m(2, 0) == 7);
    REQUIRE(m(2, 1) == 8);
    REQUIRE(m(2, 2) == 9);

    ++m;
    REQUIRE(m(0, 0) == 2);
    REQUIRE(m(0, 1) == 3);
    REQUIRE(m(0, 2) == 4);
    REQUIRE(m(1, 0) == 5);
    REQUIRE(m(1, 1) == 6);
    REQUIRE(m(1, 2) == 7);
    REQUIRE(m(2, 0) == 8);
    REQUIRE(m(2, 1) == 9);
    REQUIRE(m(2, 2) == 10);

    --m;
    REQUIRE(m(0, 0) == 1);
    REQUIRE(m(0, 1) == 2);
    REQUIRE(m(0, 2) == 3);
    REQUIRE(m(1, 0) == 4);
    REQUIRE(m(1, 1) == 5);
    REQUIRE(m(1, 2) == 6);
    REQUIRE(m(2, 0) == 7);
    REQUIRE(m(2, 1) == 8);
    REQUIRE(m(2, 2) == 9);

    m(0, 0) = 100;
    REQUIRE(m(0, 0) == 100);
    m(0, 0) = 1;

    Mat3 m2(m);
    REQUIRE(m2(0, 0) == 1);
    REQUIRE(m2(0, 1) == 2);
    REQUIRE(m2(0, 2) == 3);
    REQUIRE(m2(1, 0) == 4);
    REQUIRE(m2(1, 1) == 5);
    REQUIRE(m2(1, 2) == 6);
    REQUIRE(m2(2, 0) == 7);
    REQUIRE(m2(2, 1) == 8);
    REQUIRE(m2(2, 2) == 9);

    REQUIRE(m == m2);
    REQUIRE_FALSE(m != m2);

    REQUIRE((m + m2)(0, 0) == 2);
    REQUIRE((m + m2)(0, 1) == 4);
    REQUIRE((m + m2)(0, 2) == 6);
    REQUIRE((m + m2)(1, 0) == 8);
    REQUIRE((m + m2)(1, 1) == 10);
    REQUIRE((m + m2)(1, 2) == 12);
    REQUIRE((m + m2)(2, 0) == 14);
    REQUIRE((m + m2)(2, 1) == 16);
    REQUIRE((m + m2)(2, 2) == 18);

    REQUIRE((m + 1)(0, 0) == 2);
    REQUIRE((m + 1)(0, 1) == 3);
    REQUIRE((m + 1)(0, 2) == 4);
    REQUIRE((m + 1)(1, 0) == 5);
    REQUIRE((m + 1)(1, 1) == 6);
    REQUIRE((m + 1)(1, 2) == 7);
    REQUIRE((m + 1)(2, 0) == 8);
    REQUIRE((m + 1)(2, 1) == 9);
    REQUIRE((m + 1)(2, 2) == 10);

    REQUIRE((m + 1)(0, 0) == 2);
    REQUIRE((m + 1)(0, 1) == 3);
    REQUIRE((m + 1)(0, 2) == 4);
    REQUIRE((m + 1)(1, 0) == 5);
    REQUIRE((m + 1)(1, 1) == 6);
    REQUIRE((m + 1)(1, 2) == 7);
    REQUIRE((m + 1)(2, 0) == 8);
    REQUIRE((m + 1)(2, 1) == 9);
    REQUIRE((m + 1)(2, 2) == 10);

    REQUIRE((m - m2)(0, 0) == 0);
    REQUIRE((m - m2)(0, 1) == 0);
    REQUIRE((m - m2)(0, 2) == 0);
    REQUIRE((m - m2)(1, 0) == 0);
    REQUIRE((m - m2)(1, 1) == 0);
    REQUIRE((m - m2)(1, 2) == 0);
    REQUIRE((m - m2)(2, 0) == 0);
    REQUIRE((m - m2)(2, 1) == 0);
    REQUIRE((m - m2)(2, 2) == 0);

    REQUIRE((m - 1)(0, 0) == 0);
    REQUIRE((m - 2)(0, 1) == 0);
    REQUIRE((m - 3)(0, 2) == 0);
    REQUIRE((m - 4)(1, 0) == 0);
    REQUIRE((m - 5)(1, 1) == 0);
    REQUIRE((m - 6)(1, 2) == 0);
    REQUIRE((m - 7)(2, 0) == 0);
    REQUIRE((m - 8)(2, 1) == 0);
    REQUIRE((m - 9)(2, 2) == 0);

    REQUIRE((1 - m)(0, 0) == 0);
    REQUIRE((2 - m)(0, 1) == 0);
    REQUIRE((3 - m)(0, 2) == 0);
    REQUIRE((4 - m)(1, 0) == 0);
    REQUIRE((5 - m)(1, 1) == 0);
    REQUIRE((6 - m)(1, 2) == 0);
    REQUIRE((7 - m)(2, 0) == 0);
    REQUIRE((8 - m)(2, 1) == 0);
    REQUIRE((9 - m)(2, 2) == 0);

    REQUIRE((m * m2)(0, 0) == 30);
    REQUIRE((m * m2)(0, 1) == 36);
    REQUIRE((m * m2)(0, 2) == 42);
    REQUIRE((m * m2)(1, 0) == 66);
    REQUIRE((m * m2)(1, 1) == 81);
    REQUIRE((m * m2)(1, 2) == 96);
    REQUIRE((m * m2)(2, 0) == 102);
    REQUIRE((m * m2)(2, 1) == 126);
    REQUIRE((m * m2)(2, 2) == 150);

    REQUIRE((m * 2)(0, 0) == 2);
    REQUIRE((m * 2)(0, 1) == 4);
    REQUIRE((m * 2)(0, 2) == 6);
    REQUIRE((m * 2)(1, 0) == 8);
    REQUIRE((m * 2)(1, 1) == 10);
    REQUIRE((m * 2)(1, 2) == 12);
    REQUIRE((m * 2)(2, 0) == 14);
    REQUIRE((m * 2)(2, 1) == 16);
    REQUIRE((m * 2)(2, 2) == 18);

    REQUIRE((2 * m)(0, 0) == 2);
    REQUIRE((2 * m)(0, 1) == 4);
    REQUIRE((2 * m)(0, 2) == 6);
    REQUIRE((2 * m)(1, 0) == 8);
    REQUIRE((2 * m)(1, 1) == 10);
    REQUIRE((2 * m)(1, 2) == 12);
    REQUIRE((2 * m)(2, 0) == 14);
    REQUIRE((2 * m)(2, 1) == 16);
    REQUIRE((2 * m)(2, 2) == 18);

    REQUIRE((m / 2)(0, 0) == 0.5);
    REQUIRE((m / 2)(0, 1) == 1);
    REQUIRE((m / 2)(0, 2) == 1.5);
    REQUIRE((m / 2)(1, 0) == 2);
    REQUIRE((m / 2)(1, 1) == 2.5);
    REQUIRE((m / 2)(1, 2) == 3);
    REQUIRE((m / 2)(2, 0) == 3.5);
    REQUIRE((m / 2)(2, 1) == 4);
    REQUIRE((m / 2)(2, 2) == 4.5);

    REQUIRE((2 / m)(0, 0) == 2);
    REQUIRE((2 / m)(0, 1) == 1);
    REQUIRE((2 / m)(0, 2) == 0);
    REQUIRE((2 / m)(1, 0) == 0);
    REQUIRE((2 / m)(1, 1) == 0);
    REQUIRE((2 / m)(1, 2) == 0);
    REQUIRE((2 / m)(2, 0) == 0);
    REQUIRE((2 / m)(2, 1) == 0);
    REQUIRE((2 / m)(2, 2) == 0);
} // End Test Mat3x3

TEST_CASE("Test Mat4x4", "[Mat4x4]")
{
    Mat4 m(1,  2,  3,  4,
           5,  6,  7,  8,
           9,  10, 11, 12,
           13, 14, 15, 16);
    REQUIRE(m.a11 == 1);
    REQUIRE(m.a12 == 2);
    REQUIRE(m.a13 == 3);
    REQUIRE(m.a14 == 4);
    REQUIRE(m.a21 == 5);
    REQUIRE(m.a22 == 6);
    REQUIRE(m.a23 == 7);
    REQUIRE(m.a24 == 8);
    REQUIRE(m.a31 == 9);
    REQUIRE(m.a32 == 10);
    REQUIRE(m.a33 == 11);
    REQUIRE(m.a34 == 12);
    REQUIRE(m.a41 == 13);
    REQUIRE(m.a42 == 14);
    REQUIRE(m.a43 == 15);
    REQUIRE(m.a44 == 16);
    REQUIRE(m(0, 0) == m.a11);
    REQUIRE(m(0, 1) == m.a12);
    REQUIRE(m(0, 2) == m.a13);
    REQUIRE(m(0, 3) == m.a14);
    REQUIRE(m(1, 0) == m.a21);
    REQUIRE(m(1, 1) == m.a22);
    REQUIRE(m(1, 2) == m.a23);
    REQUIRE(m(1, 3) == m.a24);
    REQUIRE(m(2, 0) == m.a31);
    REQUIRE(m(2, 1) == m.a32);
    REQUIRE(m(2, 2) == m.a33);
    REQUIRE(m(2, 3) == m.a34);
    REQUIRE(m(3, 0) == m.a41);
    REQUIRE(m(3, 1) == m.a42);
    REQUIRE(m(3, 2) == m.a43);
    REQUIRE(m(3, 3) == m.a44);
    REQUIRE_THROWS(m(4, 1));
    REQUIRE_THROWS(m(4, 3));

    m += 1;
    REQUIRE(m(0, 0) == 2);
    REQUIRE(m(0, 1) == 3);
    REQUIRE(m(0, 2) == 4);
    REQUIRE(m(0, 3) == 5);
    REQUIRE(m(1, 0) == 6);
    REQUIRE(m(1, 1) == 7);
    REQUIRE(m(1, 2) == 8);
    REQUIRE(m(1, 3) == 9);
    REQUIRE(m(2, 0) == 10);
    REQUIRE(m(2, 1) == 11);
    REQUIRE(m(2, 2) == 12);
    REQUIRE(m(2, 3) == 13);
    REQUIRE(m(3, 0) == 14);
    REQUIRE(m(3, 1) == 15);
    REQUIRE(m(3, 2) == 16);
    REQUIRE(m(3, 3) == 17);

    m -= 1;
    REQUIRE(m(0, 0) == 1);
    REQUIRE(m(0, 1) == 2);
    REQUIRE(m(0, 2) == 3);
    REQUIRE(m(0, 3) == 4);
    REQUIRE(m(1, 0) == 5);
    REQUIRE(m(1, 1) == 6);
    REQUIRE(m(1, 2) == 7);
    REQUIRE(m(1, 3) == 8);
    REQUIRE(m(2, 0) == 9);
    REQUIRE(m(2, 1) == 10);
    REQUIRE(m(2, 2) == 11);
    REQUIRE(m(2, 3) == 12);
    REQUIRE(m(3, 0) == 13);
    REQUIRE(m(3, 1) == 14);
    REQUIRE(m(3, 2) == 15);
    REQUIRE(m(3, 3) == 16);

    m *= 2;
    REQUIRE(m(0, 0) == 2);
    REQUIRE(m(0, 1) == 4);
    REQUIRE(m(0, 2) == 6);
    REQUIRE(m(0, 3) == 8);
    REQUIRE(m(1, 0) == 10);
    REQUIRE(m(1, 1) == 12);
    REQUIRE(m(1, 2) == 14);
    REQUIRE(m(1, 3) == 16);
    REQUIRE(m(2, 0) == 18);
    REQUIRE(m(2, 1) == 20);
    REQUIRE(m(2, 2) == 22);
    REQUIRE(m(2, 3) == 24);
    REQUIRE(m(3, 0) == 26);
    REQUIRE(m(3, 1) == 28);
    REQUIRE(m(3, 2) == 30);
    REQUIRE(m(3, 3) == 32);

    m /= 2;
    REQUIRE(m(0, 0) == 1);
    REQUIRE(m(0, 1) == 2);
    REQUIRE(m(0, 2) == 3);
    REQUIRE(m(0, 3) == 4);
    REQUIRE(m(1, 0) == 5);
    REQUIRE(m(1, 1) == 6);
    REQUIRE(m(1, 2) == 7);
    REQUIRE(m(1 ,3) == 8);
    REQUIRE(m(2, 0) == 9);
    REQUIRE(m(2, 1) == 10);
    REQUIRE(m(2, 2) == 11);
    REQUIRE(m(2, 3) == 12);
    REQUIRE(m(3, 0) == 13);
    REQUIRE(m(3, 1) == 14);
    REQUIRE(m(3, 2) == 15);
    REQUIRE(m(3, 3) == 16);


    m++;
    REQUIRE(m(0, 0) == 2);
    REQUIRE(m(0, 1) == 3);
    REQUIRE(m(0, 2) == 4);
    REQUIRE(m(0, 3) == 5);
    REQUIRE(m(1, 0) == 6);
    REQUIRE(m(1, 1) == 7);
    REQUIRE(m(1, 2) == 8);
    REQUIRE(m(1, 3) == 9);
    REQUIRE(m(2, 0) == 10);
    REQUIRE(m(2, 1) == 11);
    REQUIRE(m(2, 2) == 12);
    REQUIRE(m(2, 3) == 13);
    REQUIRE(m(3, 0) == 14);
    REQUIRE(m(3, 1) == 15);
    REQUIRE(m(3, 2) == 16);
    REQUIRE(m(3, 3) == 17);

    m--;
    REQUIRE(m(0, 0) == 1);
    REQUIRE(m(0, 1) == 2);
    REQUIRE(m(0, 2) == 3);
    REQUIRE(m(0, 3) == 4);
    REQUIRE(m(1, 0) == 5);
    REQUIRE(m(1, 1) == 6);
    REQUIRE(m(1, 2) == 7);
    REQUIRE(m(1, 3) == 8);
    REQUIRE(m(2, 0) == 9);
    REQUIRE(m(2, 1) == 10);
    REQUIRE(m(2, 2) == 11);
    REQUIRE(m(2, 3) == 12);
    REQUIRE(m(3, 0) == 13);
    REQUIRE(m(3, 1) == 14);
    REQUIRE(m(3, 2) == 15);
    REQUIRE(m(3, 3) == 16);

    ++m;
    REQUIRE(m(0, 0) == 2);
    REQUIRE(m(0, 1) == 3);
    REQUIRE(m(0, 2) == 4);
    REQUIRE(m(0, 3) == 5);
    REQUIRE(m(1, 0) == 6);
    REQUIRE(m(1, 1) == 7);
    REQUIRE(m(1, 2) == 8);
    REQUIRE(m(1, 3) == 9);
    REQUIRE(m(2, 0) == 10);
    REQUIRE(m(2, 1) == 11);
    REQUIRE(m(2, 2) == 12);
    REQUIRE(m(2, 3) == 13);
    REQUIRE(m(3, 0) == 14);
    REQUIRE(m(3, 1) == 15);
    REQUIRE(m(3, 2) == 16);
    REQUIRE(m(3, 3) == 17);

    --m;
    REQUIRE(m(0, 0) == 1);
    REQUIRE(m(0, 1) == 2);
    REQUIRE(m(0, 2) == 3);
    REQUIRE(m(0, 3) == 4);
    REQUIRE(m(1, 0) == 5);
    REQUIRE(m(1, 1) == 6);
    REQUIRE(m(1, 2) == 7);
    REQUIRE(m(1, 3) == 8);
    REQUIRE(m(2, 0) == 9);
    REQUIRE(m(2, 1) == 10);
    REQUIRE(m(2, 2) == 11);
    REQUIRE(m(2, 3) == 12);
    REQUIRE(m(3, 0) == 13);
    REQUIRE(m(3, 1) == 14);
    REQUIRE(m(3, 2) == 15);
    REQUIRE(m(3, 3) == 16);

    m(0, 0) = 100;
    REQUIRE(m(0, 0) == 100);
    m(0, 0) = 1;

    Mat4 m2(m);
    REQUIRE(m2(0, 0) == 1);
    REQUIRE(m2(0, 1) == 2);
    REQUIRE(m2(0, 2) == 3);
    REQUIRE(m2(0, 3) == 4);
    REQUIRE(m2(1, 0) == 5);
    REQUIRE(m2(1, 1) == 6);
    REQUIRE(m2(1, 2) == 7);
    REQUIRE(m2(1, 3) == 8);
    REQUIRE(m2(2, 0) == 9);
    REQUIRE(m2(2, 1) == 10);
    REQUIRE(m2(2, 2) == 11);
    REQUIRE(m2(2, 3) == 12);
    REQUIRE(m2(3, 0) == 13);
    REQUIRE(m2(3, 1) == 14);
    REQUIRE(m2(3, 2) == 15);
    REQUIRE(m2(3, 3) == 16);

    REQUIRE(m == m2);
    REQUIRE_FALSE(m != m2);

    REQUIRE((m + m2)(0, 0) == 2);
    REQUIRE((m + m2)(0, 1) == 4);
    REQUIRE((m + m2)(0, 2) == 6);
    REQUIRE((m + m2)(0, 3) == 8);
    REQUIRE((m + m2)(1, 0) == 10);
    REQUIRE((m + m2)(1, 1) == 12);
    REQUIRE((m + m2)(1, 2) == 14);
    REQUIRE((m + m2)(1, 3) == 16);
    REQUIRE((m + m2)(2, 0) == 18);
    REQUIRE((m + m2)(2, 1) == 20);
    REQUIRE((m + m2)(2, 2) == 22);
    REQUIRE((m + m2)(2, 3) == 24);
    REQUIRE((m + m2)(3, 0) == 26);
    REQUIRE((m + m2)(3, 1) == 28);
    REQUIRE((m + m2)(3, 2) == 30);
    REQUIRE((m + m2)(3, 3) == 32);

    REQUIRE((m + 1)(0, 0) == 2);
    REQUIRE((m + 1)(0, 1) == 3);
    REQUIRE((m + 1)(0, 2) == 4);
    REQUIRE((m + 1)(0, 3) == 5);
    REQUIRE((m + 1)(1, 0) == 6);
    REQUIRE((m + 1)(1, 1) == 7);
    REQUIRE((m + 1)(1, 2) == 8);
    REQUIRE((m + 1)(1, 3) == 9);
    REQUIRE((m + 1)(2, 0) == 10);
    REQUIRE((m + 1)(2, 1) == 11);
    REQUIRE((m + 1)(2, 2) == 12);
    REQUIRE((m + 1)(2, 3) == 13);
    REQUIRE((m + 1)(3, 0) == 14);
    REQUIRE((m + 1)(3, 1) == 15);
    REQUIRE((m + 1)(3, 2) == 16);
    REQUIRE((m + 1)(3, 3) == 17);

    REQUIRE((m + 1)(0, 0) == 2);
    REQUIRE((m + 1)(0, 1) == 3);
    REQUIRE((m + 1)(0, 2) == 4);
    REQUIRE((m + 1)(0, 3) == 5);
    REQUIRE((m + 1)(1, 0) == 6);
    REQUIRE((m + 1)(1, 1) == 7);
    REQUIRE((m + 1)(1, 2) == 8);
    REQUIRE((m + 1)(1, 3) == 9);
    REQUIRE((m + 1)(2, 0) == 10);
    REQUIRE((m + 1)(2, 1) == 11);
    REQUIRE((m + 1)(2, 2) == 12);
    REQUIRE((m + 1)(2, 3) == 13);
    REQUIRE((m + 1)(3, 0) == 14);
    REQUIRE((m + 1)(3, 1) == 15);
    REQUIRE((m + 1)(3, 2) == 16);
    REQUIRE((m + 1)(3, 3) == 17);

    REQUIRE((m - m2)(0, 0) == 0);
    REQUIRE((m - m2)(0, 1) == 0);
    REQUIRE((m - m2)(0, 2) == 0);
    REQUIRE((m - m2)(0, 3) == 0);
    REQUIRE((m - m2)(1, 0) == 0);
    REQUIRE((m - m2)(1, 1) == 0);
    REQUIRE((m - m2)(1, 2) == 0);
    REQUIRE((m - m2)(1, 3) == 0);
    REQUIRE((m - m2)(2, 0) == 0);
    REQUIRE((m - m2)(2, 1) == 0);
    REQUIRE((m - m2)(2, 2) == 0);
    REQUIRE((m - m2)(2, 3) == 0);
    REQUIRE((m - m2)(3, 0) == 0);
    REQUIRE((m - m2)(3, 1) == 0);
    REQUIRE((m - m2)(3, 2) == 0);
    REQUIRE((m - m2)(3, 3) == 0);

    REQUIRE((m - 1)(0, 0) == 0);
    REQUIRE((m - 2)(0, 1) == 0);
    REQUIRE((m - 3)(0, 2) == 0);
    REQUIRE((m - 4)(0, 3) == 0);
    REQUIRE((m - 5)(1, 0) == 0);
    REQUIRE((m - 6)(1, 1) == 0);
    REQUIRE((m - 7)(1, 2) == 0);
    REQUIRE((m - 8)(1, 3) == 0);
    REQUIRE((m - 9)(2, 0) == 0);
    REQUIRE((m - 10)(2, 1) == 0);
    REQUIRE((m - 11)(2, 2) == 0);
    REQUIRE((m - 12)(2, 3) == 0);
    REQUIRE((m - 13)(3, 0) == 0);
    REQUIRE((m - 14)(3, 1) == 0);
    REQUIRE((m - 15)(3, 2) == 0);
    REQUIRE((m - 16)(3, 3) == 0);

    REQUIRE((1 - m)(0, 0) == 0);
    REQUIRE((2 - m)(0, 1) == 0);
    REQUIRE((3 - m)(0, 2) == 0);
    REQUIRE((4 - m)(0, 3) == 0);
    REQUIRE((5 - m)(1, 0) == 0);
    REQUIRE((6 - m)(1, 1) == 0);
    REQUIRE((7 - m)(1, 2) == 0);
    REQUIRE((8 - m)(1, 3) == 0);
    REQUIRE((9 - m)(2, 0) == 0);
    REQUIRE((10 - m)(2, 1) == 0);
    REQUIRE((11 - m)(2, 2) == 0);
    REQUIRE((12 - m)(2, 3) == 0);
    REQUIRE((13 - m)(3, 0) == 0);
    REQUIRE((14 - m)(3, 1) == 0);
    REQUIRE((15 - m)(3, 2) == 0);
    REQUIRE((16 - m)(3, 3) == 0);

    REQUIRE((m * m2)(0, 0) == 90);
    REQUIRE((m * m2)(0, 1) == 100);
    REQUIRE((m * m2)(0, 2) == 110);
    REQUIRE((m * m2)(0, 3) == 120);
    REQUIRE((m * m2)(1, 0) == 202);
    REQUIRE((m * m2)(1, 1) == 228);
    REQUIRE((m * m2)(1, 2) == 254);
    REQUIRE((m * m2)(1, 3) == 280);
    REQUIRE((m * m2)(2, 0) == 314);
    REQUIRE((m * m2)(2, 1) == 356);
    REQUIRE((m * m2)(2, 2) == 398);
    REQUIRE((m * m2)(2, 3) == 440);
    REQUIRE((m * m2)(3, 0) == 426);
    REQUIRE((m * m2)(3, 1) == 484);
    REQUIRE((m * m2)(3, 2) == 542);
    REQUIRE((m * m2)(3, 3) == 600);

    REQUIRE((m * 2)(0, 0) == 2);
    REQUIRE((m * 2)(0, 1) == 4);
    REQUIRE((m * 2)(0, 2) == 6);
    REQUIRE((m * 2)(0, 3) == 8);
    REQUIRE((m * 2)(1, 0) == 10);
    REQUIRE((m * 2)(1, 1) == 12);
    REQUIRE((m * 2)(1, 2) == 14);
    REQUIRE((m * 2)(1, 3) == 16);
    REQUIRE((m * 2)(2, 0) == 18);
    REQUIRE((m * 2)(2, 1) == 20);
    REQUIRE((m * 2)(2, 2) == 22);
    REQUIRE((m * 2)(2, 3) == 24);
    REQUIRE((m * 2)(3, 0) == 26);
    REQUIRE((m * 2)(3, 1) == 28);
    REQUIRE((m * 2)(3, 2) == 30);
    REQUIRE((m * 2)(3, 3) == 32);

    REQUIRE((2 * m)(0, 0) == 2);
    REQUIRE((2 * m)(0, 1) == 4);
    REQUIRE((2 * m)(0, 2) == 6);
    REQUIRE((2 * m)(0, 3) == 8);
    REQUIRE((2 * m)(1, 0) == 10);
    REQUIRE((2 * m)(1, 1) == 12);
    REQUIRE((2 * m)(1, 2) == 14);
    REQUIRE((2 * m)(1, 3) == 16);
    REQUIRE((2 * m)(2, 0) == 18);
    REQUIRE((2 * m)(2, 1) == 20);
    REQUIRE((2 * m)(2, 2) == 22);
    REQUIRE((2 * m)(2, 3) == 24);
    REQUIRE((2 * m)(3, 0) == 26);
    REQUIRE((2 * m)(3, 1) == 28);
    REQUIRE((2 * m)(3, 2) == 30);
    REQUIRE((2 * m)(3, 3) == 32);

    REQUIRE((m / 2)(0, 0) == 0.5);
    REQUIRE((m / 2)(0, 1) == 1);
    REQUIRE((m / 2)(0, 2) == 1.5);
    REQUIRE((m / 2)(0, 3) == 2);
    REQUIRE((m / 2)(1, 0) == 2.5);
    REQUIRE((m / 2)(1, 1) == 3);
    REQUIRE((m / 2)(1, 2) == 3.5);
    REQUIRE((m / 2)(1, 3) == 4);
    REQUIRE((m / 2)(2, 0) == 4.5);
    REQUIRE((m / 2)(2, 1) == 5);
    REQUIRE((m / 2)(2, 2) == 5.5);
    REQUIRE((m / 2)(2, 3) == 6);
    REQUIRE((m / 2)(3, 0) == 6.5);
    REQUIRE((m / 2)(3, 1) == 7);
    REQUIRE((m / 2)(3, 2) == 7.5);
    REQUIRE((m / 2)(3, 3) == 8);

    REQUIRE((2 / m)(0, 0) == 2);
    REQUIRE((2 / m)(0, 1) == 1);
    REQUIRE((2 / m)(0, 2) == 0);
    REQUIRE((2 / m)(0, 3) == 0);
    REQUIRE((2 / m)(1, 0) == 0);
    REQUIRE((2 / m)(1, 1) == 0);
    REQUIRE((2 / m)(1, 2) == 0);
    REQUIRE((2 / m)(1, 3) == 0);
    REQUIRE((2 / m)(2, 0) == 0);
    REQUIRE((2 / m)(2, 1) == 0);
    REQUIRE((2 / m)(2, 2) == 0);
    REQUIRE((2 / m)(2, 3) == 0);
    REQUIRE((2 / m)(3, 0) == 0);
    REQUIRE((2 / m)(3, 1) == 0);
    REQUIRE((2 / m)(3, 2) == 0);
    REQUIRE((2 / m)(3, 3) == 0);
} // End Test Mat4x4

/*
TEST_CASE("Test Window", "[Window]")
{
    sk_log_init();

    Window *window = sk_create_window("TempWindow", 0, 0, 800, 450);
    REQUIRE(window->is_open());
    REQUIRE(window->get_width() == 800);
    REQUIRE(window->get_height() == 450);
    REQUIRE(window->get_aspect() == 16.0f / 9.0f);
    REQUIRE(window->get_x() == 0);
    REQUIRE(window->get_y() == 0);
    window->set_width(450);
    REQUIRE(window->get_width() == 450);
    REQUIRE(window->get_aspect() == 1);
    window->set_height(800);
    REQUIRE(window->get_height() == 800);
    REQUIRE(window->get_aspect() == 9.0f / 16.0f);
    REQUIRE(window->get_id() == 1);
    window->set_x(200);
    REQUIRE(window->get_x() == 200);
    window->set_y(200);
    REQUIRE(window->get_y() == 200);
    window->close();
    window->set_size_and_pos(500, 500, 100, 200);
    REQUIRE(window->get_width() == 450);
    REQUIRE(window->get_height() == 800);
    REQUIRE(window->get_aspect() == 9.0f / 16.0f);
    REQUIRE(window->get_x() == 200);
    REQUIRE(window->get_y() == 200);
    REQUIRE_FALSE(window->is_open());

    sk_log_shutdown();
} // End Test Window

/*
TEST_CASE("General Application Test", "[Application]")
{
    class ShutdownTestLayer : public ApplicationLayer
    {
    public:
        void update() override
        {
            application_shutdown();
        }
    };

    log_init();
    REQUIRE_NOTHROW(application_add_layer(new_unique<ShutdownTestLayer>()));
    REQUIRE_NOTHROW(application_shutdown());
    REQUIRE_NOTHROW(application_run());
    REQUIRE_NOTHROW(application_init());
    REQUIRE_NOTHROW(application_run());
    log_shutdown();
} // End General Application Test

TEST_CASE("Test Layers", "[ApplicationLayer]")
{
    static bool layer_started = false;
    static bool layer_ended = false;
    static bool layer_updated = false;

    class AppTestLayer : public ApplicationLayer
    {
    public:
        void start() override
        {
            layer_started = true;
        }

        void end() override
        {
            layer_ended = true;
        }

        void update() override
        {
            layer_updated = true;
            application_shutdown();
        }
    };

    log_init();
    REQUIRE_NOTHROW(application_add_layer(new_unique<AppTestLayer>()));
    REQUIRE_NOTHROW(application_init());
    REQUIRE_NOTHROW(application_run());
    REQUIRE(layer_started == true);
    REQUIRE(layer_ended == true);
    REQUIRE(layer_updated == true);
    log_shutdown();
} // End Test Layers

TEST_CASE("Test Event", "[Application::OnEvent]")
{
    static bool event_recieved = false;

    class EventTestLayer : public ApplicationLayer
    {
    public:
        void update() override
        {
            Event e = { TEST_EVENT };
            application_on_event(e);
            application_shutdown();
        }

        void on_event(Event& e)
        {
            if (e.type == TEST_EVENT)
            {
                event_recieved = true;
            }
        }
    };

    log_init();
    REQUIRE_NOTHROW(application_add_layer(new_unique<EventTestLayer>()));
    REQUIRE_NOTHROW(application_init());
    REQUIRE_NOTHROW(application_run());
    REQUIRE(event_recieved == true);
    log_shutdown();
} // End Test Event
*/
