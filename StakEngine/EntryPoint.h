#pragma once

#include "Types.h"
#include "Application.h"
#include "Platform.h"
#include "Log.h"

extern application_spec GetAppSpec();

inline i32 StakMain() {
  LogInit();

  application_spec Spec = GetAppSpec();
  application *App = ApplicationInit(&Spec);
  ApplicationRun(App);
  ApplicationShutdown(&App);

  LogShutdown();
  return 0;
}

#ifndef SK_NO_MAIN
#pragma warning (disable : 4067)
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
