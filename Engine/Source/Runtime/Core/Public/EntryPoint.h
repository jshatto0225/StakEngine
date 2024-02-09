#pragma once

#include "Application.h"
#include "Log.h"
#include "Types.h"

extern void extern_init();

inline i32 stak_main() {
  sk::Log::init();
  extern_init();
  sk::Application::init();
  sk::Application::run();
  sk::Log::shutdown();
  return 0;
}

#ifndef SK_NO_MAIN
#if defined(SK_DEBUG) or defined(SK_RELEASE)
int main(int argc, char **argv) { return stak_main(); }
#elif defined SK_DIST
#ifdef WIN32
#include <windows.h>
i32 WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR commandLine, i32 showCommand) {
  return stak_main();
}
#endif
#endif
#else
void extern_init() {}
#endif
