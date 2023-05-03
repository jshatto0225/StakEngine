#pragma once

#include "StakEngine/Core/Log.h"

#define STK_CORE_ASSERT(x, message) if(!x) { STK_CORE_FATAL(message); __debugbreak();}

#define STK_ASSERT(x, message) if(!x) { STK_FATAL(message); __debugbreak();}