#pragma once

#include "Types.h"
#include "Application.h"
#include "Platform.h"

extern sk::Application *GetApp();

inline i32 StakMain() {
  sk::Log::Init();

  sk::Application *app = GetApp();
  app->run();
  delete app;

  sk::Log::Shutdown();
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
