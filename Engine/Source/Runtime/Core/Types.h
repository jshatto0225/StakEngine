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
using Ref = std::shared_ptr<T>;

template<typename T, typename ... Args>
Ref<T> MakeRef(Args&&... args) {
  return std::make_shared<T>(std::forward<Args>(args)...);
}

template<typename T>
using Scope = std::unique_ptr<T>;

template<typename T, typename ... Args>
Scope<T> MakeScope(Args&&... args) {
  return std::make_unique<T>(std::forward<Args>(args)...);
}

// Aliases because c++ is ugly af
namespace sk {
struct Event;
using EventFn = std::function<void(Event &)>;
}

#define BIND_METHOD(fn) [this](auto&&... args) -> decltype(auto) { \
  return this->fn(std::forward<decltype(args)>(args)...); \
}
