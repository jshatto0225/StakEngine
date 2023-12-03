#pragma once

#define SK_DEBUG

#include "Application.h"
#include "Types.h"
#include "Log.h"

i32 main()
{
    Log::Init();
    SK_CORE_INFO("Log Initialized");
    Application::Init();
    Application::Run();
    Log::Shutdown();
    return 0;
}