#pragma once

#include <stdio.h>

#if defined(SK_WINDOWS)
#define DEBUG_BREAK __debugbreak
#endif

#ifdef SK_DEBUG
#define ASSERT(x, message) \
do { \
  if (!(x)) { \
    fprintf(stderr, "Assertion Failed: %s, File: %s, Line: %d", message, __FILE__, __LINE__); \
    DEBUG_BREAK(); \
  } \
} while(0)
#else
#define ASSERT(x) x
#endif