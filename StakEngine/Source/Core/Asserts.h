#pragma once

#include <assert.h>

#ifdef SK_DEBUG
    #define ASSERT(x) assert(x);
#else
    #define ASSERT(x) x
#endif
