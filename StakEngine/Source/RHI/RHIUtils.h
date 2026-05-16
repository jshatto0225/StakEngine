#pragma once

#include "RHI.h"

#include <functional>

#define DEFER_CONCAT_INNER(x, y) x ## y
#define DEFER_CONCAT(x, y) DEFER_CONCAT_INNER(x, y)

#define defer ::rhi::Defer DEFER_CONCAT(_defer_var_, __LINE__); DEFER_CONCAT(_defer_var_, __LINE__).func = [&]() 

namespace rhi {

struct Defer {
    Defer() {}
    ~Defer() { func(); }

    Defer(const Defer &) = delete;
    Defer &operator=(const Defer &) = delete;

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
struct Set {
    T *data;
    u64 count;
    u64 capacity;
};

template<typename K, typename V>
struct Map {
    K *keys;
    V *values;
    u64 count;
    u64 capacity;
};

struct String {
    char *data;
    u64 length;
};

// String
// Not recommended for strings that will be modified after creation or exit the scope it was created in.
String str_from_cstr(const char *cstr, RHIAllocator *alloc_callbacks);

String str_from_cstr_clone(const char *cstr, RHIAllocator *alloc_callbacks);

void str_free(String *str, RHIAllocator *alloc_callbacks);

// Array
template<typename T>
Array<T> array_from_carr(const T *carr, u64 count, RHIAllocator *alloc_callbacks);

template<typename T>
Array<T> array(u64 count, RHIAllocator *alloc_callbacks);

template<typename T>
Array<T> array_clone(Array<T> arr, RHIAllocator *alloc_callbacks);

template<typename T>
void array_free(Array<T> *arr, RHIAllocator *alloc_callbacks);

// Dynamic Array
template<typename T>
DynamicArray<T> dyn_array_from_carr_clone(const T *carr, u64 count, RHIAllocator *alloc_callbacks);

template<typename T>
DynamicArray<T> dyn_array(u64 size, u64 capacity, RHIAllocator *alloc_callbacks);

template<typename T>
DynamicArray<T> dyn_array_clone(DynamicArray<T> arr, RHIAllocator *alloc_callbacks);

template<typename T>
void dyn_array_free(DynamicArray<T> *arr, RHIAllocator *alloc_callbacks);

template<typename T>
void dyn_array_push_back(DynamicArray<T> *arr, T item, RHIAllocator *alloc_callbacks);

template<typename T>
void dyn_array_pop_back(DynamicArray<T> *arr);

template<typename T>
T *dyn_array_back(DynamicArray<T> *arr);

template<typename T>
void dyn_array_push_front(DynamicArray<T> *arr, T item, RHIAllocator *alloc_callbacks);

template<typename T>
void dyn_array_pop_front(DynamicArray<T> *arr);

template<typename T>
T *dyn_array_front(DynamicArray<T> *arr);

template<typename T>
void dyn_array_clear(DynamicArray<T> *arr, RHIAllocator *alloc_callbacks);

template<typename T>
void dyn_array_reserve(DynamicArray<T> *arr, u64 new_capacity, RHIAllocator *alloc_callbacks);

template<typename T>
void dyn_array_resize(DynamicArray<T> *arr, u64 new_size, RHIAllocator *alloc_callbacks);

template<typename T>
void dyn_array_insert(DynamicArray<T> *arr, u64 index, T item, RHIAllocator *alloc_callbacks);

// Set
template<typename T>
Set<T> set_from_carr_clone(const T *carr, u64 count, RHIAllocator *alloc_callbacks);

template<typename T>
Set<T> set(u64 size, u64 capacity, RHIAllocator *alloc_callbacks);

template<typename T>
Set<T> set_clone(Set<T> set, RHIAllocator *alloc_callbacks);

template<typename T>
void set_free(Set<T> *set, RHIAllocator *alloc_callbacks);

template<typename T>
void set_insert(Set<T> *set, T item, RHIAllocator *alloc_callbacks);

template<typename T>
void set_erase(Set<T> *set, T item, RHIAllocator *alloc_callbacks);

template<typename T>
bool set_contains(Set<T> *set, T item);

// Map
template<typename K, typename V>
Map<K, V> map_from_carr_clone(const K *ckeys, const V *cvalues, u64 count, RHIAllocator *alloc_callbacks);

template<typename K, typename V>
Map<K, V> map(u64 size, u64 capacity, RHIAllocator *alloc_callbacks);

template<typename K, typename V>
Map<K, V> map_clone(Map<K, V> map, RHIAllocator *alloc_callbacks);

template<typename K, typename V>
void map_free(Map<K, V> *map, RHIAllocator *alloc_callbacks);

template<typename K, typename V>
void map_insert(Map<K, V> *map, K key, V value, RHIAllocator *alloc_callbacks);

template<typename K, typename V>
void map_erase(Map<K, V> *map, K key, RHIAllocator *alloc_callbacks);

template<typename K, typename V>
bool map_contains(Map<K, V> *map, K key);

}