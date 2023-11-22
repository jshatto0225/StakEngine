#pragma once

#include <cstdint>
#include <memory>

typedef double f64;
typedef float f32;

typedef int64_t i64;
typedef int32_t i32;
typedef int16_t i16;
typedef int8_t i8;

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

// TODO: Rethink smart pointer names

template<typename T>
using Shared = std::shared_ptr<T>;

template<typename T, typename ...Args>
constexpr Shared<T> MakeShared(Args&& ...args)
{
    return std::make_shared<T>(std::forward<Args>(args)...);
}

template<typename T>
using Unique = std::unique_ptr<T>;

template<typename T, typename ...Args>
constexpr Unique<T> MakeUnique(Args&& ...args)
{
    return std::make_unique<T>(std::forward<Args>(args)...);
}