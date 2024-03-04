#pragma once

#include <stdint.h>
#include <memory>
#include <string>
#include <vector>
#include <functional>

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

template<typename T>
using Shared = std::shared_ptr<T>;

template<typename T, typename ... Args>
Shared<T> MakeShared(Args&&... args) {
  return std::make_shared<T>(std::forward<Args>(args)...);
}

template<typename T>
using Unique = std::unique_ptr<T>;

template<typename T, typename ... Args>
Unique<T> MakeUnique(Args&&... args) {
  return std::make_unique<T>(std::forward<Args>(args)...);
}

template<typename T>
using Weak = std::weak_ptr<T>;

constexpr auto Placeholder1 = std::placeholders::_1;
constexpr auto Placeholder2 = std::placeholders::_2;
constexpr auto Placeholder3 = std::placeholders::_3;
constexpr auto Placeholder4 = std::placeholders::_4;

struct Event;
using EventFn = std::function<void(Event &)>;

#define BIND_METHOD(fn) [this](auto&&... args) -> decltype(auto) { \
  return this->fn(std::forward<decltype(args)>(args)...); \
}
