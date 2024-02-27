#pragma once

#include "Types.h"
#include "Application.h"

sk::Application *get_app();

inline i32 sk_main() {
  sk::Application *app = get_app();
  app->run();
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
  return _sk_main();
}
#endif
#endif
#else
void extern_init() {}
#endif
