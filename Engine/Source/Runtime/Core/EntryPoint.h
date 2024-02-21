#pragma once

#include "Log.h"

extern void extern_init();
extern void sk_app_init();
extern void sk_app_run();
extern void sk_app_shutdown();

inline i32 sk_main() {
  //sk_log_init();
  extern_init();
  sk_app_init();
  sk_app_run();
  //sk_log_shutdown();
  return 0;
}

#ifndef SK_NO_MAIN
#if defined(SK_DEBUG) or defined(SK_RELEASE)
int main(int argc, char **argv) {
  return sk_main();
}
#elif defined SK_DIST
#ifdef WIN32
#include <windows.h>
i32 WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR commandLine, i32 showCommand) {
  return sk_main();
}
#endif
#endif
#else
void extern_init() {}
#endif
