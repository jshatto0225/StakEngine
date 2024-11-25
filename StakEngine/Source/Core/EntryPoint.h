#pragma once

#include "FApplication.h"
#include "FLog.h"
#include "Types.h"

extern FApplication *CreateApplication();

inline FSInt32 StakMain() {
  FLog::Init();

  FApplication *App = CreateApplication();
  App->Run();
  delete App;

  return 0;
}

int main(int argc, char **argv) {
  return StakMain();
}

