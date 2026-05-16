#pragma once

// This file is not C compatible and is only intended to be used by the RHI backends

// All dynamically sized containers (Map, Set, DynamicArray) will check if alloc->free is present.
// If it is free/alloc may be used on resize, else we don't free the old data.
// Based on the RHI allocator spec one allocator should be permanent and have an alloc/free pair the other should be temp and have an alloc/reset.
// The structs in here do not care weather they get a temp allocator or permantnt allocator.
// If a permanent allocator is used the objects will free all of the memory they use, if a temp allocator is used
//   it is up to the caller to free that memory.

#include "RHI.h"

#include <functional>

#define DEFER_CONCAT_INNER(x, y) x ## y
#define DEFER_CONCAT(x, y) DEFER_CONCAT_INNER(x, y)

#define defer ::rhi::Defer DEFER_CONCAT(_defer_var_, __LINE__); DEFER_CONCAT(_defer_var_, __LINE__).func = [&]() 

#define for_set(it, set) \
    for (u64 i = 0; i < (set).capacity; ++i) \
        if ((set).data[i].present && ((it) = &(set).data[i].value, true))

#define for_map(k, v, map) \
    for (u64 i = 0; i < (map).capacity; ++i) \
        if ((map).data[i].present && \
            ((k) = &(map).data[i].key, true) && \
            ((v) = &(map).data[i].value, true))

namespace rhi {

struct Defer {
    Defer() = default;
    ~Defer() { func(); }

    Defer(const Defer &) = delete;
    Defer &operator=(const Defer &) = delete;

    Defer(Defer &&) = delete;
    Defer &operator=(Defer &&) = delete;

    std::function<void()> func;
};

template<typename T>
struct Array {
    T *data;
    u64 count;
};

template<typename T>
struct DynamicArray {
    T *data;
    u64 count;
    u64 capacity;
};

template<typename T>
struct SetSlot {
    T value;
    bool present;
};

template<typename T>
struct Set {
    SetSlot<T> *data;
    u64 count;
    u64 capacity;
};

template<typename K, typename V>
struct MapSlot {
    K key;
    V value;
    bool present;
};

template<typename K, typename V>
struct Map {
    MapSlot<K, V> *data;
    u64 count;
    u64 capacity;
};

struct String {
    char *data;
    u64 length;
};

// String
// Not recommended for strings that will be modified after creation or exit the scope it was created in.
String str_from_cstr(const char *cstr, RHIAllocator *alloc);

String str_from_cstr_clone(const char *cstr, RHIAllocator *alloc);

void str_free(String *str, RHIAllocator *alloc);

// Array
template<typename T>
Array<T> array_from_carr(const T *carr, u64 count, RHIAllocator *alloc);

template<typename T>
Array<T> array(u64 count, RHIAllocator *alloc);

template<typename T>
Array<T> array_clone(Array<T> arr, RHIAllocator *alloc);

template<typename T>
void array_free(Array<T> *arr, RHIAllocator *alloc);

// Dynamic Array
template<typename T>
DynamicArray<T> dyn_array_from_carr_clone(const T *carr, u64 count, RHIAllocator *alloc);

template<typename T>
DynamicArray<T> dyn_array(u64 size, u64 capacity, RHIAllocator *alloc);

template<typename T>
DynamicArray<T> dyn_array_clone(DynamicArray<T> arr, RHIAllocator *alloc);

template<typename T>
void dyn_array_free(DynamicArray<T> *arr, RHIAllocator *alloc);

template<typename T>
void dyn_array_push_back(DynamicArray<T> *arr, T item, RHIAllocator *alloc);

template<typename T>
void dyn_array_pop_back(DynamicArray<T> *arr);

template<typename T>
T *dyn_array_back(DynamicArray<T> *arr);

template<typename T>
void dyn_array_push_front(DynamicArray<T> *arr, T item, RHIAllocator *alloc);

template<typename T>
void dyn_array_pop_front(DynamicArray<T> *arr);

template<typename T>
T *dyn_array_front(DynamicArray<T> *arr);

template<typename T>
void dyn_array_clear(DynamicArray<T> *arr, RHIAllocator *alloc);

template<typename T>
void dyn_array_reserve(DynamicArray<T> *arr, u64 new_capacity, RHIAllocator *alloc);

template<typename T>
void dyn_array_resize(DynamicArray<T> *arr, u64 new_size, RHIAllocator *alloc);

template<typename T>
void dyn_array_insert(DynamicArray<T> *arr, u64 index, T item, RHIAllocator *alloc);

// Set
template<typename T>
Set<T> set_from_carr_clone(const T *carr, u64 count, RHIAllocator *alloc);

template<typename T>
Set<T> set(u64 size, u64 capacity, RHIAllocator *alloc);

template<typename T>
Set<T> set_clone(Set<T> set, RHIAllocator *alloc);

template<typename T>
void set_free(Set<T> *set, RHIAllocator *alloc);

template<typename T>
void set_insert(Set<T> *set, T item, RHIAllocator *alloc);

template<typename T>
void set_erase(Set<T> *set, T item, RHIAllocator *alloc);

template<typename T>
bool set_contains(Set<T> *set, T item);

// Map

/**
 * FNV-1a
 * 
 * Generic hash over any struct. hash_bytes(&struct, sizeof(struct)).
 * Still need to do a memcmp over the struct bytes.
 * RHIAllocator spec of ZII makes padding bytes irrelevant.
 * 
 * uint64_t hash_bytes(const void *data, size_t size) {
 *     const uint8_t *bytes = (const uint8_t *)data;
 *
 *     uint64_t hash = 14695981039346656037ull; // FNV offset basis
 *
 *     for (size_t i = 0; i < size; i++) {
 *         hash ^= (uint64_t)bytes[i];
 *         hash *= 1099511628211ull; // FNV prime
 *     }
 *
 *     return hash;
 * }
 */

template<typename K, typename V>
Map<K, V> map_from_carr_clone(const K *ckeys, const V *cvalues, u64 count, RHIAllocator *alloc);

template<typename K, typename V>
Map<K, V> map(u64 capacity, RHIAllocator *alloc);

template<typename K, typename V>
Map<K, V> map_clone(Map<K, V> map, RHIAllocator *alloc);

template<typename K, typename V>
void map_free(Map<K, V> *map, RHIAllocator *alloc);

template<typename K, typename V>
void map_insert(Map<K, V> *map, K key, V value, RHIAllocator *alloc);

template<typename K, typename V>
void map_erase(Map<K, V> *map, K key, RHIAllocator *alloc);

template<typename K, typename V>
bool map_contains(Map<K, V> *map, K key);

template<typename K, typename V>
V *map_get(Map<K, V> *map, K key);

}
