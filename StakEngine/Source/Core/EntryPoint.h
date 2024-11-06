#pragma once

#include "Application.h"
#include "Log.h"
#include "Types.h"

extern Stak::Application *createApplication();

inline i32 stakMain() {
  Stak::Log::init();

  Stak::Application *app = createApplication();
  app->run();

  delete app;

  return 0;
}

int main(int argc, char **argv) {
  return stakMain();
}

