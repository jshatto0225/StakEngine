#pragma once

#include "FApplication.h"
#include "FLog.h"
#include "Types.h"

extern TRef<FApplication> CreateApplication();

inline FSInt32 StakMain() {
  FLog::Init();

  TRef<FApplication> App = CreateApplication();
  App->Run();

  App = NULL;

  return 0;
}

int main(int argc, char **argv) {
  return StakMain();
}

