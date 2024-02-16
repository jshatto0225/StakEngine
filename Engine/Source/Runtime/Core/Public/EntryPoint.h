#pragma once

#include "Application.h"
#include "Log.h"
#include "Types.h"

extern void extern_init();

inline i32 stak_main() {
  sk::log_init();
  extern_init();
  sk::application_init();
  sk::application_run();
  sk::log_shutdown();
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
