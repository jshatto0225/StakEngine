#pragma once

#include "Application.h"
#include "Types.h"
#include "Log.h"

extern void ExternInit();

inline i32 StakMain()
{
    SK::Log::Init();
    ExternInit();
    SK::Application::Init();
    SK::Application::Run();
    SK::Log::Shutdown();
    return 0;
}

#ifndef SK_NO_MAIN
    #if defined(SK_DEBUG) or defined(SK_RELEASE)
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
                i32 showCommand)
            {
                return StakMain();
            }
        #endif
    #endif
#else
void ExternInit() {}
#endif
