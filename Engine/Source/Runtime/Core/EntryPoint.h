#pragma once

#include "Types.h"
#include "Application.h"
#include "Platform.h"

Application *GetApp();

inline i32 StakMain() {
  Log::Init();

  Application *app = GetApp();
  app->run();
  delete app;

  Log::Shutdown();
  return 0;
}

#ifndef SK_NO_MAIN
#if defined(SK_DEBUG) or defined(SK_RELEASE)
int main(int argc, char **argv) {
  return StakMain();
}
#elif defined(SK_DIST)
#ifdef SK_WINDOWS
#include <windows.h>
i32 WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR commandLine, i32 showCommand) {
  return StakMain();
}
#endif
#endif
#endif
