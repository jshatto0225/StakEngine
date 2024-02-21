#pragma once

#include <stdio.h>

#ifdef SK_DEBUG
#define SK_CORE_TRACE(...) fprintf(stdout, __VA_ARGS__)
#define SK_CORE_INFO(...) fprintf(stdout, __VA_ARGS__)
#define SK_CORE_WARN(...) fprintf(stderr, __VA_ARGS__)
#define SK_CORE_ERROR(...) fprintf(stderr, __VA_ARGS__)
#define SK_CORE_CRITICAL(...) fprintf(stderr, __VA_ARGS__)

#define SK_TRACE(...) fprintf(stdout, __VA_ARGS__)
#define SK_INFO(...) fprintf(stdout, __VA_ARGS__)
#define SK_WARN(...) fprintf(stderr, __VA_ARGS__)
#define SK_ERROR(...) fprintf(stderr, __VA_ARGS__)
#define SK_CRITICAL(...) fprintf(stderr, __VA_ARGS__)
#else
#define SK_CORE_TRACE(...)
#define SK_CORE_INFO(...)
#define SK_CORE_WARN(...)
#define SK_CORE_ERROR(...)
#define SK_CORE_CRITICAL(...)

#define SK_TRACE(...)
#define SK_INFO(...)
#define SK_WARN(...)
#define SK_ERROR(...)
#define SK_CRITICAL(...)
#endif
