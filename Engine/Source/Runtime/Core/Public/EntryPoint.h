#pragma once

#include "Application.h"
#include "Types.h"
#include "Log.h"

i32 StakMain()
{
    Application::Init();
    Application::Run();
    return 0;
}

#ifndef SK_NO_MAIN
#ifdef SK_DEBUG or SK_RELEASE
int main(int argc, char** argv)
{
    return StakMain();
}
#elif defined SK_DIST
#ifdef WIN32
#include <windows.h>
i32 WINAPI WinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR commandLine,
    showCommand)
{
    return StakMain();
}
#endif
#endif
#endif
