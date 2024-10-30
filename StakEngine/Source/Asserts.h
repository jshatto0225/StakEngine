#pragma once

#include <assert.h>

#if defined(SK_WINDOWS)
#define DEBUG_BREAK __debugbreak
#endif

#ifdef SK_DEBUG
    #define ASSERT(x) assert(x);
#else
    #define ASSERT(x) x
#endif
