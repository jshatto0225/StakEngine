#pragma once

#include <stdint.h>
#include <memory>

typedef float FFloat;
typedef double FDouble;

typedef int64_t FSInt64;
typedef int32_t FSInt32;
typedef int16_t FSInt16;
typedef int8_t  FSInt8;

typedef uint64_t FUInt64;
typedef uint32_t FUInt32;
typedef uint16_t FUInt16;
typedef uint8_t  FUInt8;

typedef bool FBool;

typedef FUInt64 FOpaqueHandle;

template <typename TType> using TScope = std::unique_ptr<TType>;
template <typename TType, typename... Args>
constexpr TScope<TType> TCreateScope(Args &&...args) {
  return std::make_unique<TType>(std::forward<Args>(args)...);
}

template <typename TType> using TRef = std::shared_ptr<TType>;
template <typename TType, typename... Args>
constexpr TRef<TType> TCreateRef(Args &&...args) {
  return std::make_shared<TType>(std::forward<Args>(args)...);
}

#define BIT(x) (1 << x)
