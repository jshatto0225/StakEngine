#pragma once

#include "Types.h"
#include "FInput.h"

class IWindow;

struct FWindowResizeEvent {
  const FSInt32 WIDTH;
  const FSInt32 HEIGHT;
};

struct FKeyEvent {
  const EKeyCode Key;
  const EInputState State;
};

struct FMouseButtonEvent {
  const EMouseCode Button;
  const EInputState State;
};

struct FMouseMoveEvent {
  const FFloat X;
  const FFloat Y;
};
