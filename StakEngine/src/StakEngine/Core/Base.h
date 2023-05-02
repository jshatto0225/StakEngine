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

// Might not need ----------------------------------
#ifdef STK_PLATFORM_WINDOWS
	#ifdef STK_BUILD_DLL
		#define STAK_API __declspec(dllexport)
	#else
		#define STAK_API __declspec(dllimport)
	#endif
#endif
// -------------------------------------------------

#include "Log.h"