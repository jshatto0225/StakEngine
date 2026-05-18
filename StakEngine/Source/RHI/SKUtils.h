#pragma once

// This file is not C compatible and is only intended to be used by the SK backends

// All dynamically sized containers (Map, Set, DynamicArray) will check if alloc->free is present.
// If it is free/alloc may be used on resize, else we don't free the old data.
// Based on the SK allocator spec one allocator should be permanent and have an alloc/free pair the other should be temp and have an alloc/reset.
// The structs in here do not care weather they get a temp allocator or permantnt allocator.
// If a permanent allocator is used the objects will free all of the memory they use, if a temp allocator is used
//   it is up to the caller to free that memory.

#include "SK.h"

#include <functional>
#include <assert.h>

#define DEFER_CONCAT_INNER(x, y) x ## y
#define DEFER_CONCAT(x, y) DEFER_CONCAT_INNER(x, y)

#define defer ::sk::Defer DEFER_CONCAT(_defer_var_, __LINE__); DEFER_CONCAT(_defer_var_, __LINE__).func = [&]() 

#define for_set(it, set) \
    for (u64 i = 0; i < (set).capacity; ++i) \
        if ((set).data[i].present && ((it) = &(set).data[i].value, true))

#define for_map(k, v, map) \
    for (u64 i = 0; i < (map).capacity; ++i) \
        if ((map).data[i].present && \
            ((k) = &(map).data[i].key, true) && \
            ((v) = &(map).data[i].value, true))

namespace sk {

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

template<typename T>
struct Ringbuffer {
    T *data;
    u64 head;
    u64 tail;
    u64 count;
    u64 capacity;
};

// String
inline String str_from_cstr(const char *cstr, SKAllocator *alloc) {
    assert(cstr);
    assert(alloc);
    assert(strlen(cstr));

    String s;
    s.length = strlen(cstr);
    s.data = (char *) alloc->alloc(sizeof(char) * s.length, alloc->user_data);
    memcpy(s.data, cstr, s.length);
    return s;
}

inline void str_free(String *str, SKAllocator *alloc) {
    assert(str);
    assert(alloc);
    assert(str->length);

    if (alloc->free) {
        alloc->free(str->data, alloc->user_data);
    }
    str->length = 0;
    str->data = nullptr;
}

// Array
template<typename T>
inline Array<T> array_from_carr(const T *carr, u64 count, SKAllocator *alloc) {
    assert(carr);
    assert(alloc);
    assert(count);

    Array<T> arr = {};
    arr.count = count;
    arr.data = (T *) alloc->alloc(sizeof(T) * count, alloc->user_data);
    memcpy(arr.data, carr, count * sizeof(T));
    return arr;
}

template<typename T>
inline Array<T> array(u64 count, SKAllocator *alloc) {
    assert(alloc);
    assert(count);

    Array<T> arr = {};
    arr.count = count;
    arr.data = (T *) alloc->alloc(sizeof(T) * count, alloc->user_data);
    return arr;
}

template<typename T>
inline Array<T> array_clone(Array<T> *arr, SKAllocator *alloc) {
    assert(alloc);
    assert(arr);
    assert(arr->data);
    assert(arr->count);

    Array<T> new_arr = {};
    new_arr.count = arr->count;
    new_arr.data = (T *) alloc->alloc(sizeof(T) * new_arr.count, alloc->user_data);
    memcpy(new_arr.data, arr->data, new_arr.count * sizeof(T));
    return new_arr;
}

template<typename T>
inline void array_free(Array<T> *arr, SKAllocator *alloc) {
    assert(arr);
    assert(alloc);
    assert(arr->data);
    assert(arr->count);

    if (alloc->free) {
        alloc->free(arr->data, alloc->user_data);
    }
    arr->count = 0;
    arr->data = nullptr;
}

// Dynamic Array
template<typename T>
inline DynamicArray<T> dyn_array_from_carr(const T *carr, u64 count, SKAllocator *alloc) {
    assert(carr);
    assert(alloc);

    DynamicArray<T> arr = {};
    arr.count = count;
    arr.capacity = count * 2;
    if (!arr.capacity) {
        return arr;
    }
    arr.data = (T *) alloc->alloc(sizeof(T) * arr.capacity, alloc->user_data);
    memcpy(arr.data, carr, count * sizeof(T));
    return arr;
}

template<typename T>
inline DynamicArray<T> dyn_array(u64 size, u64 capacity, SKAllocator *alloc) {
    assert(alloc);
    assert(size <= capacity);
    assert(capacity > 1);

    DynamicArray<T> arr = {};
    arr.count = size;
    arr.capacity = capacity;
    if (!arr.capacity) {
        return arr;
    }
    arr.data = (T *) alloc->alloc(sizeof(T) * arr.capacity, alloc->user_data);
    return arr;
}

template<typename T>
inline DynamicArray<T> dyn_array_clone(DynamicArray<T> *arr, SKAllocator *alloc) {
    assert(arr);
    assert(alloc);

    DynamicArray<T> new_arr = {};
    new_arr.count = arr->count;
    new_arr.capacity = arr->capacity;
    if (!new_arr.capacity) {
        return new_arr;
    }
    new_arr.data = (T *) alloc->alloc(sizeof(T) * new_arr.capacity, alloc->user_data);
    memcpy(new_arr.data, arr->data, sizeof(T) * new_arr.count);
    return new_arr;
}

template<typename T>
inline void dyn_array_free(DynamicArray<T> *arr, SKAllocator *alloc) {
    assert(arr);
    assert(alloc);

    if (alloc->free) {
        alloc->free(arr->data, alloc->user_data);
    }
    arr->count = 0;
    arr->capacity = 0;
    arr->data = nullptr;
}

template<typename T>
inline void dyn_array_push_back(DynamicArray<T> *arr, T item, SKAllocator *alloc) {
    assert(arr);
    assert(alloc);

    if (arr->count == arr->capacity) {
        u64 reserve_size = arr->count * 2;
        if (!reserve_size) {
            reserve_size = 2;
        }
        dyn_array_reserve(arr, reserve_size, alloc);
    }

    arr->data[arr->count] = item;
    arr->count += 1;
}

template<typename T>
inline void dyn_array_pop_back(DynamicArray<T> *arr) {
    assert(arr);
    assert(arr->count);

    arr->count -= 1;
}

template<typename T>
inline T *dyn_array_back(DynamicArray<T> *arr) {
    assert(arr);
    assert(arr->count);

    return arr->data[arr->count - 1];
}

template<typename T>
inline void dyn_array_clear(DynamicArray<T> *arr) {
    assert(arr);
    arr->count = 0;
}

template<typename T>
inline void dyn_array_reserve(DynamicArray<T> *arr, u64 new_capacity, SKAllocator *alloc) {
    assert(arr);
    assert(alloc);
    assert(new_capacity > 1);

    if (new_capacity < arr->capacity) {
        return;
    }

    arr->capacity = new_capacity;

    T *new_data = (T *) alloc->alloc(sizeof(T) * arr->capacity, alloc->user_data);
    memcpy(new_data, arr->data, arr->count);
    if (alloc->free) {
        alloc->free(arr->data, alloc->user_data);
    }
    arr->data = new_data;
}

template<typename T>
inline void dyn_array_resize(DynamicArray<T> *arr, u64 new_size, SKAllocator *alloc) {
    assert(arr);
    assert(alloc);

    if (new_size <= arr->size) {
        arr->size = new_size;
        return;
    }

    if (new_size > arr->capacity) {
        u64 reserve_size = new_size * 2;
        dyn_array_reserve(arr, reserve_size, alloc);
    }

    arr->count = new_size;
}

template<typename T>
inline void dyn_array_insert(DynamicArray<T> *arr, u64 index, T item, SKAllocator *alloc) {
    assert(arr);
    assert(alloc);
    assert(index < arr->count);

    if (arr->count == arr->capacity) {
        u64 reserve_size = arr->capacity * 2;
        dyn_array_reserve(arr, reserve_size, alloc);
    }

    for (u64 i = arr->count; i > index; i--) {
        arr->data[i] = arr->data[i - 1];
    }

    arr->data[index] = item;
}

/**
    * FNV-1a
    *
    * Generic hash over any struct. hash_bytes(&struct, sizeof(struct)).
    * Still need to do a memcmp over the struct bytes.
    * SKAllocator spec of ZII makes padding bytes irrelevant.
    */
uint64_t hash_bytes(const void *data, size_t size) {
    const uint8_t *bytes = (const uint8_t *) data;
    uint64_t hash = 14695981039346656037ull; // FNV offset basis
    for (size_t i = 0; i < size; i++) {
        hash ^= (uint64_t) bytes[i];
        hash *= 1099511628211ull; // FNV prime
    }
    return hash;
}

// Set
inline u64 next_pow2_u64(u64 v) {
    if (v <= 1) {
        return 1;
    }

    v--;

    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v |= v >> 32;

    v++;

    return v;
}

template<typename T>
inline Set<T> set_from_carr(const T *carr, u64 count, SKAllocator *alloc) {
    assert(carr);
    assert(alloc);

    Set<T> set = {};
    set.capacity = next_pow2_u64(count * 2);
    set.count = 0;
    if (!set.capacity) {
        return set;
    }
    set.data = (SetSlot<T> *) alloc->alloc(set.capacity * sizeof(SetSlot<T>), alloc->user_data);

    for (u64 i = 0; i < count; i++) {
        set_insert(&set, carr[i], alloc);
    }

    return set;
}

template<typename T>
inline Set<T> set(u64 capacity, SKAllocator *alloc) {
    assert(alloc);

    Set<T> set = {};
    set.capacity = capacity;
    if (!set.capacity) {
        return set;
    }
    set.data = (SetSlot<T> *) alloc->alloc(set.capacity * sizeof(SetSlot<T>), alloc->user_data);
    return set;
}

template<typename T>
inline Set<T> set_clone(Set<T> *set, SKAllocator *alloc) {
    assert(set);
    assert(alloc);

    Set<T> new_set = {};
    new_set.capacity = set->capacity;
    new_set.count = set->count;
    if (new_set.capacity == 0) {
        return new_set;
    }
    new_set.data = (SetSlot<T> *) alloc->alloc(sizeof(SetSlot<T>) * new_set.capacity, alloc->user_data);
    memcpy(new_set.data, set->data, new_set.count * sizeof(SetSlot<T>));
    return new_set;
}

template<typename T>
inline void set_free(Set<T> *set, SKAllocator *alloc) {
    assert(set);
    assert(alloc);

    if (alloc->free) {
        alloc->free(set->data, alloc->user_data);
    }
    set->capacity = 0;
    set->count = 0;
}

template<typename T>
inline void set_reserve_and_rehash(Set<T> *set, u64 capacity, SKAllocator *alloc) {
    assert(set);
    assert(alloc);

    if (capacity <= set->capacity) {
        return;
    }

    SetSlot<T> *new_items = (SetSlot<T> *) alloc->alloc(sizeof(SetSlot<T>) * capacity, alloc->user_data);
    SetSlot<T> *old_items = set->data;
    u64 old_capacity = set->capacity;
    u64 old_count = set->count;

    set->count = 0;
    set->data = new_items;
    set->capacity = capacity;

    for (u64 i = 0; i < old_capacity; i++) {
        if (!old_items[i].present) {
            continue;
        }

        set_insert(set, old_items[i].value, alloc);
    }

    assert(set->count == old_count);

    alloc->free(old_items, alloc->user_data);
}

template<typename T>
inline void set_insert(Set<T> *set, T item, SKAllocator *alloc) {
    assert(set);
    assert(alloc);

    if (set->capacity == set->count) {
        u64 reserve_size = next_pow2_u64(set->capacity * 2);
        set_reserve_and_rehash(set, reserve_size, alloc);
    }

    u64 hash = hash_bytes(&item, sizeof(T));
    u64 mask = set->capacity - 1;
    u64 index = (u64) hash & mask;

    for (;;) {
        if (!set->data[index].present) {
            set->data[index].present = true;
            set->data[index].value = item;
            set->count += 1;
            return;
        }

        if (memcmp(&set->data[index].value, &item, sizeof(T)) == 0) {
            return;
        }

        index = (index + 1) & mask;
    }
}

template<typename T>
inline void set_erase(Set<T> *set, T item, SKAllocator *alloc) {
    assert(set);
    assert(alloc);

    if (set->capacity == 0) {
        return;
    }

    u64 hash = hash_bytes(&item, sizeof(T));
    u64 mask = set->capacity - 1;
    u64 index = (u64) hash & mask;

    for (;;) {
        if (!set->data[index].present) {
            return;
        }

        if (memcmp(&set->data[index].value, &item, sizeof(T)) == 0) {
            set->data[index].present = false;
            set->count -= 1;

            u64 next = (index + 1) & mask;

            while (set->data[next].present) {
                T reinsert_item = set->data[next].value;

                set->data[next].present = false;
                set->count -= 1;

                set_insert(set, reinsert_item, alloc);

                next = (next + 1) & mask;
            }

            return;
        }
    }
}

template<typename T>
inline bool set_contains(Set<T> *set, T item) {
    if (set->count == 0) {
        return false;
    }

    u64 hash = hash_bytes(&item, sizeof(T));
    u64 mask = set->capacity - 1;
    u64 index = (u64) hash & mask;

    for (;;) {
        if (!set->data[index].present) {
            return false;
        }
        if (memcmp(&set->data[index].value, &item, sizeof(T)) == 0) {
            return true;
        }
        index = (index + 1) & mask;
    }
}

// Map
template<typename K, typename V>
inline Map<K, V> map_from_carr(const K *ckeys, const V *cvalues, u64 count, SKAllocator *alloc) {
    assert(ckeys);
    assert(cvalues);
    assert(alloc);

    Map<K, V> map = {};

    map.capacity = next_pow2_u64(count * 2);
    map.count = 0;

    if (!map.capacity) {
        return map;
    }

    map.data = (MapSlot<K, V> *) alloc->alloc(sizeof(MapSlot<K, V>) * map.capacity, alloc->user_data);

    for (u64 i = 0; i < count; i++) {
        map_insert(&map, ckeys[i], cvalues[i], alloc);
    }

    return map;
}

template<typename K, typename V>
inline Map<K, V> map(u64 capacity, SKAllocator *alloc) {
    assert(alloc);

    Map<K, V> map = {};

    map.capacity = next_pow2_u64(capacity);

    if (!map.capacity) {
        return map;
    }

    map.data = (MapSlot<K, V> *) alloc->alloc(sizeof(MapSlot<K, V>) * map.capacity, alloc->user_data);

    return map;
}

template<typename K, typename V>
inline Map<K, V> map_clone(Map<K, V> *map, SKAllocator *alloc) {
    assert(alloc);
    assert(map);

    Map<K, V> new_map = {};

    new_map.capacity = map->capacity;
    new_map.count = map->count;

    if (!new_map.capacity) {
        return new_map;
    }

    new_map.data = (MapSlot<K, V> *) alloc->alloc(sizeof(MapSlot<K, V>) * new_map.capacity, alloc->user_data);

    memcpy(new_map.data, map->data, sizeof(MapSlot<K, V>) * new_map.capacity);

    return new_map;
}

template<typename K, typename V>
inline void map_free(Map<K, V> *map, SKAllocator *alloc) {
    assert(map);
    assert(alloc);

    if (map->data && alloc->free) {
        alloc->free(map->data, alloc->user_data);
    }

    map->data = nullptr;
    map->capacity = 0;
    map->count = 0;
}

template<typename K, typename V>
inline void map_reserve_and_rehash(Map<K, V> *map, u64 capacity, SKAllocator *alloc) {
    assert(map);
    assert(alloc);

    if (capacity <= map->capacity) {
        return;
    }

    capacity = next_pow2_u64(capacity);

    MapSlot<K, V> *new_data = (MapSlot<K, V> *) alloc->alloc(sizeof(MapSlot<K, V>) * capacity, alloc->user_data);

    MapSlot<K, V> *old_data = map->data;

    u64 old_capacity = map->capacity;
    u64 old_count = map->count;

    map->data = new_data;
    map->capacity = capacity;
    map->count = 0;

    for (u64 i = 0; i < old_capacity; i++) {
        if (!old_data[i].present) {
            continue;
        }

        map_insert(map, old_data[i].key, old_data[i].value, alloc);
    }

    assert(map->count == old_count);

    if (old_data && alloc->free) {
        alloc->free(old_data, alloc->user_data);
    }
}

template<typename K, typename V>
inline void map_insert(Map<K, V> *map, K key, V value, SKAllocator *alloc) {
    assert(map);
    assert(alloc);

    if (map->count == map->capacity) {
        u64 reserve_size = map->capacity * 2;

        if (!reserve_size) {
            reserve_size = 2;
        }

        map_reserve_and_rehash(map, reserve_size, alloc);
    }

    u64 hash = hash_bytes(&key, sizeof(K));

    u64 mask = map->capacity - 1;
    u64 index = hash & mask;

    for (;;) {
        if (!map->data[index].present) {
            map->data[index].present = true;
            map->data[index].key = key;
            map->data[index].value = value;
            map->count += 1;
            return;
        }

        if (memcmp(&map->data[index].key, &key, sizeof(K)) == 0) {
            map->data[index].value = value;
            return;
        }

        index = (index + 1) & mask;
    }
}

template<typename K, typename V>
inline void map_erase(Map<K, V> *map, K key, SKAllocator *alloc) {
    assert(map);
    assert(alloc);

    if (map->capacity == 0) {
        return;
    }

    u64 hash = hash_bytes(&key, sizeof(K));

    u64 mask = map->capacity - 1;
    u64 index = hash & mask;

    for (;;) {
        if (!map->data[index].present) {
            return;
        }

        if (memcmp(&map->data[index].key, &key, sizeof(K)) == 0) {
            map->data[index].present = false;
            map->count -= 1;

            u64 next = (index + 1) & mask;

            while (map->data[next].present) {
                K reinsert_key = map->data[next].key;
                V reinsert_value = map->data[next].value;

                map->data[next].present = false;
                map->count -= 1;

                map_insert(map, reinsert_key, reinsert_value, alloc);

                next = (next + 1) & mask;
            }

            return;
        }

        index = (index + 1) & mask;
    }
}

template<typename K, typename V>
inline bool map_contains(Map<K, V> *map, K key) {
    assert(map);

    if (map->count == 0) {
        return false;
    }

    u64 hash = hash_bytes(&key, sizeof(K));

    u64 mask = map->capacity - 1;
    u64 index = hash & mask;

    for (;;) {
        if (!map->data[index].present) {
            return false;
        }

        if (memcmp(&map->data[index].key, &key, sizeof(K)) == 0) {
            return true;
        }

        index = (index + 1) & mask;
    }
}

template<typename K, typename V>
inline V *map_get(Map<K, V> *map, K key) {
    assert(map);

    assert(map->count > 0);

    u64 hash = hash_bytes(&key, sizeof(K));

    u64 mask = map->capacity - 1;
    u64 index = hash & mask;

    for (;;) {
        assert(map->data[index].present);

        if (memcmp(&map->data[index].key, &key, sizeof(K)) == 0) {
            return &map->data[index].value;
        }

        index = (index + 1) & mask;
    }
}

// Ringbuffer
template<typename T>
inline Ringbuffer<T> ringbuffer_from_carr(const T *carr, u64 count, SKAllocator *alloc) {
    assert(carr);
    assert(alloc);

    Ringbuffer<T> rb = {};
    if (count == 0) {
        rb.data = nullptr;
        rb.capacity = 0;
        rb.head = rb.tail = rb.count = 0;
        return rb;
    }

    rb.capacity = count * 2;
    if (!rb.capacity) {
        rb.data = nullptr;
        rb.head = rb.tail = rb.count = 0;
        return rb;
    }

    rb.data = (T *) alloc->alloc(sizeof(T) * rb.capacity, alloc->user_data);
    memcpy(rb.data, carr, sizeof(T) * count);
    rb.head = 0;
    rb.count = count;
    rb.tail = count % rb.capacity;
    return rb;
}

template<typename T>
inline Ringbuffer<T> ringbuffer(u64 capacity, SKAllocator *alloc) {
    assert(alloc);

    Ringbuffer<T> rb = {};
    rb.capacity = capacity;
    if (!rb.capacity) {
        rb.data = nullptr;
        rb.head = rb.tail = rb.count = 0;
        return rb;
    }

    rb.data = (T *) alloc->alloc(sizeof(T) * rb.capacity, alloc->user_data);
    rb.head = rb.tail = rb.count = 0;
    return rb;
}

template<typename T>
inline Ringbuffer<T> ringbuffer_clone(const Ringbuffer<T> *src, SKAllocator *alloc) {
    assert(src);
    assert(alloc);

    Ringbuffer<T> rb = {};
    rb.capacity = src->capacity;
    rb.count = src->count;
    rb.head = 0;
    rb.tail = src->count % rb.capacity;

    if (!rb.capacity) {
        rb.data = nullptr;
        return rb;
    }

    rb.data = (T *) alloc->alloc(sizeof(T) * rb.capacity, alloc->user_data);
    if (src->count == 0) return rb;

    if (src->head < src->tail) {
        memcpy(rb.data, &src->data[src->head], sizeof(T) * src->count);
    } else {
        u64 first = src->capacity - src->head;
        memcpy(rb.data, &src->data[src->head], sizeof(T) * first);
        memcpy(rb.data + first, src->data, sizeof(T) * src->tail);
    }
    return rb;
}

template<typename T>
inline void ringbuffer_free(Ringbuffer<T> *rb, SKAllocator *alloc) {
    assert(rb);
    assert(alloc);

    if (rb->data && alloc->free) {
        alloc->free(rb->data, alloc->user_data);
    }
    rb->data = nullptr;
    rb->head = rb->tail = rb->count = rb->capacity = 0;
}

template<typename T>
inline void ringbuffer_clear(Ringbuffer<T> *rb) {
    assert(rb);
    rb->head = rb->tail = rb->count = 0;
}

template<typename T>
inline bool ringbuffer_empty(const Ringbuffer<T> *rb) {
    assert(rb);
    return rb->count == 0;
}

template<typename T>
inline u64 ringbuffer_size(const Ringbuffer<T> *rb) {
    assert(rb);
    return rb->count;
}

template<typename T>
inline u64 ringbuffer_capacity(const Ringbuffer<T> *rb) {
    assert(rb);
    return rb->capacity;
}

template<typename T>
inline void ringbuffer_relayout_into(const Ringbuffer<T> *rb, T *dst) {
    assert(rb);
    assert(dst);
    if (rb->count == 0) return;
    if (rb->head < rb->tail) {
        memcpy(dst, &rb->data[rb->head], sizeof(T) * rb->count);
    } else {
        u64 first = rb->capacity - rb->head;
        memcpy(dst, &rb->data[rb->head], sizeof(T) * first);
        memcpy(dst + first, rb->data, sizeof(T) * rb->tail);
    }
}

template<typename T>
inline void ringbuffer_reserve(Ringbuffer<T> *rb, u64 new_capacity, SKAllocator *alloc) {
    assert(rb);
    assert(alloc);
    if (new_capacity <= rb->capacity) return;
    if (new_capacity == 0) new_capacity = 2;

    T *new_data = (T *) alloc->alloc(sizeof(T) * new_capacity, alloc->user_data);
    if (rb->count) {
        ringbuffer_relayout_into(rb, new_data);
    }
    if (rb->data && alloc->free) {
        alloc->free(rb->data, alloc->user_data);
    }
    rb->data = new_data;
    rb->capacity = new_capacity;
    rb->head = 0;
    rb->tail = rb->count % rb->capacity;
}

template<typename T>
inline void ringbuffer_ensure_capacity(Ringbuffer<T> *rb, u64 min_capacity, SKAllocator *alloc) {
    assert(rb);
    assert(alloc);
    if (rb->capacity >= min_capacity) return;
    u64 cap = rb->capacity ? rb->capacity * 2 : 2;
    while (cap < min_capacity) cap *= 2;
    ringbuffer_reserve(rb, cap, alloc);
}

// push_back: append at tail (grows when full)
template<typename T>
inline void ringbuffer_push_back(Ringbuffer<T> *rb, const T &value, SKAllocator *alloc) {
    assert(rb);
    assert(alloc);
    if (!rb->data || rb->count == rb->capacity) {
        ringbuffer_ensure_capacity(rb, rb->capacity ? rb->capacity + 1 : 2, alloc);
    }
    rb->data[rb->tail] = value;
    rb->tail = (rb->tail + 1) % rb->capacity;
    rb->count++;
}

// pop_back: remove last element (returns false if empty)
template<typename T>
inline bool ringbuffer_pop_back(Ringbuffer<T> *rb) {
    assert(rb);
    if (!rb->data || rb->count == 0) return false;
    rb->tail = (rb->tail + rb->capacity - 1) % rb->capacity;
    rb->count--;
    return true;
}

// push_front: prepend at head (grows when full)
template<typename T>
inline void ringbuffer_push_front(Ringbuffer<T> *rb, const T &value, SKAllocator *alloc) {
    assert(rb);
    assert(alloc);
    if (!rb->data || rb->count == rb->capacity) {
        ringbuffer_ensure_capacity(rb, rb->capacity ? rb->capacity + 1 : 2, alloc);
    }
    rb->head = (rb->head + rb->capacity - 1) % rb->capacity;
    rb->data[rb->head] = value;
    rb->count++;
}

// pop_front: remove first element (returns false if empty)
template<typename T>
inline bool ringbuffer_pop_front(Ringbuffer<T> *rb) {
    assert(rb);
    if (!rb->data || rb->count == 0) return false;
    rb->head = (rb->head + 1) % rb->capacity;
    rb->count--;
    return true;
}

template<typename T>
inline T *ringbuffer_front(Ringbuffer<T> *rb) {
    assert(rb);
    if (!rb->data || rb->count == 0) return nullptr;
    return &rb->data[rb->head];
}

template<typename T>
inline T *ringbuffer_back(Ringbuffer<T> *rb) {
    assert(rb);
    if (!rb->data || rb->count == 0) return nullptr;
    u64 idx = (rb->tail + rb->capacity - 1) % rb->capacity;
    return &rb->data[idx];
}

}