#pragma once

#include <memory>
#include <cstdint>
#include <cassert>
#include <string>
#include <optional>
#include <set>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <utility>

// NOTE: defer macro from https://gist.github.com/p2004a/045726d70a490d12ad62
// SPDX-FileCopyrightText: 2015 Marek Rusinowski
// SPDX-License-Identifier: MIT
template<typename F>
class defer_finalizer {
    F f;
    bool moved;
  public:
    template<typename T>
    defer_finalizer(T && f_) : f(std::forward<T>(f_)), moved(false) { }

    defer_finalizer(const defer_finalizer &) = delete;

    defer_finalizer(defer_finalizer && other) : f(std::move(other.f)), moved(other.moved) {
        other.moved = true;
    }

    ~defer_finalizer() {
        if (!moved) f();
    }
};

struct {
    template<typename F>
    defer_finalizer<F> operator<<(F && f) {
        return defer_finalizer<F>(std::forward<F>(f));
    }
} deferrer;

#define TOKENPASTE(x, y) x ## y
#define TOKENPASTE2(x, y) TOKENPASTE(x, y)
#define defer auto TOKENPASTE2(__deferred_lambda_call, __COUNTER__) = deferrer << [&]
// End defer

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

template<typename T>
using Array = std::vector<T>;

template<typename T1, typename T2>
using Hash_Map = std::unordered_map<T1, T2>;

template<typename T>
using Hash_Set = std::unordered_set<T>;

template<typename T1, typename T2>
using Map = std::map<T1, T2>;

template<typename T>
using Set = std::set<T>;

template<typename T>
using Option = std::optional<T>;

typedef std::string String;

#define For_Index(index, max) for (u32 index = 0; index < (u32)max; index++)
#define ForI(max) For_Index(i, max)
#define ForJ(max) For_Index(j, max)
#define ForK(max) For_Index(k, max)

#define For(max) ForI(max)

#define It(iterable) for (auto &it : iterable)
#define CIt(iterable) for (const auto &it : iterable)

#define Inc(index, start, end) for (u32 index = (u32)start; index < (u32)end; index++)
#define Dec(index, start, end) for (u32 index = (u32)start; index > (u32)end; index--)

#ifdef SK_DEBUG
#define ASSERT(x) assert(x);
#else
#define ASSERT(x)
#endif

// I got this from TheCherno
template <typename T>
using Scope = std::unique_ptr<T>;

template <typename T, typename... Args>
constexpr Scope<T> create_scope(Args &&...args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
}

template <typename T>
using Ref = std::shared_ptr<T>;

template <typename T, typename... Args>
constexpr Ref<T> create_ref(Args &&...args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
}

template <typename T>
using Weak = std::weak_ptr<T>;

#define rescope std::move
#define ptr_cast std::static_pointer_cast
