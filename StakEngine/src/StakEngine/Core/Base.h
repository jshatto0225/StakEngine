#pragma once

#include <stdint.h>

typedef long long STKll;
typedef unsigned int STKuint;
typedef int STKint;
typedef unsigned char STKbytes;
typedef unsigned char STKuchar;
typedef float STKfloat;
typedef double STKdouble;
typedef void STKvoid;
typedef bool STKbool;
typedef char STKchar;

#define BIT(x) (1<<x)

#define STK_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

#include "Log.h"

#define STK_ASSERT(x, message) if(!x)\
{\
	STK_CORE_CRITICAL(message);\
	__debugbreak();\
}