#pragma once

#include "Application.h"
#include "Log.h"
#include "Types.h"

extern Stak::Application *CreateApplication();

inline i32 StakMain() {
  Stak::Log::Init();

  Stak::Application *app = CreateApplication();
  app->Run();

  delete app;

  return 0;
}

int main(int argc, char **argv) { 
  return StakMain(); 
}

