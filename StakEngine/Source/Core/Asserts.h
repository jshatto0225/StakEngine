#pragma once

#include <cassert>

#ifdef SK_DEBUG
    #define ASSERT(x) assert(x);
#else
    #define ASSERT(x) x
#endif
