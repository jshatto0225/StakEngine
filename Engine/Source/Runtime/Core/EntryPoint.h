#pragma once

#include "Types.h"

extern void extern_init();

void sk_app_init();
void sk_app_run();

inline i32 sk_main() {
  extern_init();
  sk_app_init();
  sk_app_run();
  return 0;
}

#ifndef SK_NO_MAIN
#if defined(SK_DEBUG) or defined(SK_RELEASE)
int main(int argc, char **argv) {
  return sk_main();
}
#elif defined(SK_DIST)
#ifdef SK_WINDOWS
#include <windows.h>
i32 WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR commandLine, i32 showCommand) {
  return sk_main();
}
#endif
#endif
#else
void extern_init() {}
#endif
