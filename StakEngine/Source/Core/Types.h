#pragma once

#include <cstdint>
#include <memory>

typedef float f32;
typedef double f64;

typedef int64_t s64;
typedef int32_t s32;
typedef int16_t s16;
typedef int8_t  s8;

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef u64 Handle;

template <typename Type> using Scope = std::unique_ptr<Type>;
template <typename Type, typename... Args>
constexpr Scope<Type> create_scope(Args &&...args) {
    return std::make_unique<Type>(std::forward<Args>(args)...);
}

template <typename Type> using Ref = std::shared_ptr<Type>;
template <typename Type, typename... Args>
constexpr Ref<Type> create_ref(Args &&...args) {
    return std::make_shared<Type>(std::forward<Args>(args)...);
}

#define BIT(x) (1 << x)
