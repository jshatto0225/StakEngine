#pragma once

#include "Types.h"
#include "Application.h"
#include "Platform.h"
#include "Log.h"

/**
 * @brief User defined initialization fuinction
 * 
 * @return application_spec Specification used to start the app
 */
extern application_spec GetAppSpec();

/**
 * @brief Applications main function
 * 
 * @return i32 Standard C/C++ return codes
 */
inline i32
StakMain()
{
    LogInit();

    application_spec Spec = GetAppSpec();
    ApplicationInit(&Spec);
    ApplicationRun();
    ApplicationShutdown();

    LogShutdown();

    return 0;
}

#ifndef SK_NO_MAIN
#pragma warning (disable : 4067)
#if defined(SK_DEBUG) or defined(SK_RELEASE)
/**
 * @brief Debug entry point
 */
int
main(int argc, char **argv)
{
    return StakMain();
}
#elif defined(SK_DIST)
#ifdef SK_WINDOWS
#include <windows.h>
/**
 * @brief Win32 release entry point
 */
i32 WINAPI
WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR commandLine, i32 showCommand)
{
    return StakMain();
}
#endif
#endif
#endif
