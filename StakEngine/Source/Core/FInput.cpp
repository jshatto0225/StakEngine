#include "FInput.h"

#include "FLog.h"
#undef DELETE

FInput::FInput() {
  ASSERT(!sInstance);
  sInstance = this;

  mKeys[EKeyCode::UNKNOWN] = EInputState::UP;
  mKeys[EKeyCode::SPACE] = EInputState::UP;
  mKeys[EKeyCode::APOSTROPHE] = EInputState::UP;
  mKeys[EKeyCode::COMMA] = EInputState::UP;
  mKeys[EKeyCode::MINUS] = EInputState::UP;
  mKeys[EKeyCode::PERIOD] = EInputState::UP;
  mKeys[EKeyCode::SLASH] = EInputState::UP;
  mKeys[EKeyCode::ZERO] = EInputState::UP;
  mKeys[EKeyCode::ONE] = EInputState::UP;
  mKeys[EKeyCode::TWO] = EInputState::UP;
  mKeys[EKeyCode::THREE] = EInputState::UP;
  mKeys[EKeyCode::FOUR] = EInputState::UP;
  mKeys[EKeyCode::FIVE] = EInputState::UP;
  mKeys[EKeyCode::SIX] = EInputState::UP;
  mKeys[EKeyCode::SEVEN] = EInputState::UP;
  mKeys[EKeyCode::EIGHT] = EInputState::UP;
  mKeys[EKeyCode::NINE] = EInputState::UP;
  mKeys[EKeyCode::SEMICOLON] = EInputState::UP;
  mKeys[EKeyCode::EQUAL] = EInputState::UP;
  mKeys[EKeyCode::A] = EInputState::UP;
  mKeys[EKeyCode::B] = EInputState::UP;
  mKeys[EKeyCode::C] = EInputState::UP;
  mKeys[EKeyCode::D] = EInputState::UP;
  mKeys[EKeyCode::E] = EInputState::UP;
  mKeys[EKeyCode::F] = EInputState::UP;
  mKeys[EKeyCode::G] = EInputState::UP;
  mKeys[EKeyCode::H] = EInputState::UP;
  mKeys[EKeyCode::I] = EInputState::UP;
  mKeys[EKeyCode::J] = EInputState::UP;
  mKeys[EKeyCode::K] = EInputState::UP;
  mKeys[EKeyCode::L] = EInputState::UP;
  mKeys[EKeyCode::M] = EInputState::UP;
  mKeys[EKeyCode::N] = EInputState::UP;
  mKeys[EKeyCode::O] = EInputState::UP;
  mKeys[EKeyCode::P] = EInputState::UP;
  mKeys[EKeyCode::Q] = EInputState::UP;
  mKeys[EKeyCode::R] = EInputState::UP;
  mKeys[EKeyCode::S] = EInputState::UP;
  mKeys[EKeyCode::T] = EInputState::UP;
  mKeys[EKeyCode::U] = EInputState::UP;
  mKeys[EKeyCode::V] = EInputState::UP;
  mKeys[EKeyCode::W] = EInputState::UP;
  mKeys[EKeyCode::X] = EInputState::UP;
  mKeys[EKeyCode::Y] = EInputState::UP;
  mKeys[EKeyCode::Z] = EInputState::UP;
  mKeys[EKeyCode::LEFT_BRACKET] = EInputState::UP;
  mKeys[EKeyCode::BACKSLASH] = EInputState::UP;
  mKeys[EKeyCode::RIGHT_BRACKET] = EInputState::UP;
  mKeys[EKeyCode::GRAVE_ACCENT] = EInputState::UP;
  mKeys[EKeyCode::WORLD_1] = EInputState::UP;
  mKeys[EKeyCode::WORLD_2] = EInputState::UP;
  mKeys[EKeyCode::ESCAPE] = EInputState::UP;
  mKeys[EKeyCode::ENTER] = EInputState::UP;
  mKeys[EKeyCode::TAB] = EInputState::UP;
  mKeys[EKeyCode::BACKSPACE] = EInputState::UP;
  mKeys[EKeyCode::INSERT] = EInputState::UP;
  mKeys[EKeyCode::DELETE] = EInputState::UP;
  mKeys[EKeyCode::RIGHT] = EInputState::UP;
  mKeys[EKeyCode::LEFT] = EInputState::UP;
  mKeys[EKeyCode::DOWN] = EInputState::UP;
  mKeys[EKeyCode::UP] = EInputState::UP;
  mKeys[EKeyCode::PAGE_UP] = EInputState::UP;
  mKeys[EKeyCode::PAGE_DOWN] = EInputState::UP;
  mKeys[EKeyCode::HOME] = EInputState::UP;
  mKeys[EKeyCode::END] = EInputState::UP;
  mKeys[EKeyCode::CAPS_LOCK] = EInputState::UP;
  mKeys[EKeyCode::SCROLL_LOCK] = EInputState::UP;
  mKeys[EKeyCode::NUM_LOCK] = EInputState::UP;
  mKeys[EKeyCode::PRINT_SCREEN] = EInputState::UP;
  mKeys[EKeyCode::PAUSE] = EInputState::UP;
  mKeys[EKeyCode::F1] = EInputState::UP;
  mKeys[EKeyCode::F2] = EInputState::UP;
  mKeys[EKeyCode::F3] = EInputState::UP;
  mKeys[EKeyCode::F4] = EInputState::UP;
  mKeys[EKeyCode::F5] = EInputState::UP;
  mKeys[EKeyCode::F6] = EInputState::UP;
  mKeys[EKeyCode::F7] = EInputState::UP;
  mKeys[EKeyCode::F8] = EInputState::UP;
  mKeys[EKeyCode::F9] = EInputState::UP;
  mKeys[EKeyCode::F10] = EInputState::UP;
  mKeys[EKeyCode::F11] = EInputState::UP;
  mKeys[EKeyCode::F12] = EInputState::UP;
  mKeys[EKeyCode::F13] = EInputState::UP;
  mKeys[EKeyCode::F14] = EInputState::UP;
  mKeys[EKeyCode::F15] = EInputState::UP;
  mKeys[EKeyCode::F16] = EInputState::UP;
  mKeys[EKeyCode::F17] = EInputState::UP;
  mKeys[EKeyCode::F18] = EInputState::UP;
  mKeys[EKeyCode::F19] = EInputState::UP;
  mKeys[EKeyCode::F20] = EInputState::UP;
  mKeys[EKeyCode::F21] = EInputState::UP;
  mKeys[EKeyCode::F22] = EInputState::UP;
  mKeys[EKeyCode::F23] = EInputState::UP;
  mKeys[EKeyCode::F24] = EInputState::UP;
  mKeys[EKeyCode::F25] = EInputState::UP;
  mKeys[EKeyCode::KP_0] = EInputState::UP;
  mKeys[EKeyCode::KP_1] = EInputState::UP;
  mKeys[EKeyCode::KP_2] = EInputState::UP;
  mKeys[EKeyCode::KP_3] = EInputState::UP;
  mKeys[EKeyCode::KP_4] = EInputState::UP;
  mKeys[EKeyCode::KP_5] = EInputState::UP;
  mKeys[EKeyCode::KP_6] = EInputState::UP;
  mKeys[EKeyCode::KP_7] = EInputState::UP;
  mKeys[EKeyCode::KP_8] = EInputState::UP;
  mKeys[EKeyCode::KP_9] = EInputState::UP;
  mKeys[EKeyCode::KP_DECIMAL] = EInputState::UP;
  mKeys[EKeyCode::KP_DIVIDE] = EInputState::UP;
  mKeys[EKeyCode::KP_MULTIPLY] = EInputState::UP;
  mKeys[EKeyCode::KP_SUBTRACT] = EInputState::UP;
  mKeys[EKeyCode::KP_ADD] = EInputState::UP;
  mKeys[EKeyCode::KP_ENTER] = EInputState::UP;
  mKeys[EKeyCode::KP_EQUAL] = EInputState::UP;
  mKeys[EKeyCode::LEFT_SHIFT] = EInputState::UP;
  mKeys[EKeyCode::LEFT_CONTROL] = EInputState::UP;
  mKeys[EKeyCode::LEFT_ALT] = EInputState::UP;
  mKeys[EKeyCode::LEFT_SUPER] = EInputState::UP;
  mKeys[EKeyCode::RIGHT_SHIFT] = EInputState::UP;
  mKeys[EKeyCode::RIGHT_CONTROL] = EInputState::UP;
  mKeys[EKeyCode::RIGHT_ALT] = EInputState::UP;
  mKeys[EKeyCode::RIGHT_SUPER] = EInputState::UP;
  mKeys[EKeyCode::MENU] = EInputState::UP;
  mKeys[EKeyCode::LAST] = EInputState::UP;

  mMouseButtons[EMouseCode::ONE] = EInputState::UP;
  mMouseButtons[EMouseCode::TWO] = EInputState::UP;
  mMouseButtons[EMouseCode::THREE] = EInputState::UP;
  mMouseButtons[EMouseCode::FOUR] = EInputState::UP;
  mMouseButtons[EMouseCode::FIVE] = EInputState::UP;
  mMouseButtons[EMouseCode::SIX] = EInputState::UP;
  mMouseButtons[EMouseCode::SEVEN] = EInputState::UP;
  mMouseButtons[EMouseCode::EIGHT] = EInputState::UP;
  mMouseButtons[EMouseCode::LAST] = EInputState::UP;
  mMouseButtons[EMouseCode::LEFT] = EInputState::UP;
  mMouseButtons[EMouseCode::RIGHT] = EInputState::UP;
  mMouseButtons[EMouseCode::MIDDLE] = EInputState::UP;

  mIsUsingRawInput = false;
  mCursorVisibility = ECursorVisibility::NORMAL;
}

FInput::~FInput() {
  sInstance = NULL;
}

void FInput::AddKeyStateCallback(EInputState State, EKeyCode Key, std::function<void()> Func) {
  switch (State) {
  case EInputState::DOWN:
    mKeyPressCallbacks[Key].push_back(Func);
    break;
  case EInputState::UP:
    mKeyReleaseCallbacks[Key].push_back(Func);
    break;
  }
}

void FInput::AddGenericKeyStateCallback(EInputState State, std::function<void(EKeyCode)> Func) {
  switch (State) {
  case EInputState::DOWN:
    mGenericKeyPressCallbacks.push_back(Func);
    break;
  case EInputState::UP:
    mGenericKeyReleaseCallbacks.push_back(Func);
    break;
  }
}

void FInput::AddGenericKeyCallback(std::function<void(EInputState, EKeyCode)> Func) {
  mGenericKeyCallbacks.push_back(Func);
}

void FInput::AddMouseButtonStateCallback(EInputState State, EMouseCode Button, std::function<void()> Func) {
  switch (State) {
  case EInputState::DOWN:
    mMouseButtonPressCallbacks[Button].push_back(Func);
    break;
  case EInputState::UP:
    mMouseButtonReleaseCallbacks[Button].push_back(Func);
    break;
  }
}

void FInput::AddGenericMouseButtonStateCallback(EInputState State, std::function<void(EMouseCode)> Func) {
  switch (State) {
  case EInputState::DOWN:
    mGenericMouseButtonPressCallbacks.push_back(Func);
    break;
  case EInputState::UP:
    mGenericMouseButtonReleaseCallbacks.push_back(Func);
    break;
  }
}

void FInput::AddGenericMouseButtonCallback(std::function<void(EInputState, EMouseCode)> Func) {
  mGenericMouseButtonCallbacks.push_back(Func);
}

void FInput::AddMouseMoveCallback(std::function<void(FFloat, FFloat)> Func) {
  mMouseMoveCallbacks.push_back(Func);
}

void FInput::SetKey(EKeyCode Key, EInputState State) {
  mKeys[Key] = State;

  switch (State) {
  case EInputState::DOWN:
    for (const auto &Func : mKeyPressCallbacks[Key]) {
      Func();
    }
    for (const auto &Func : mGenericKeyPressCallbacks) {
      Func(Key);
    }
    break;
  case EInputState::UP:
    for (const auto &Func : mKeyReleaseCallbacks[Key]) {
      Func();
    }
    for (const auto &Func : mGenericKeyReleaseCallbacks) {
      Func(Key);
    }
    break;
  }

  for (const auto &Func : mGenericKeyCallbacks) {
    Func(State, Key);
  }
}

void FInput::SetMouseButton(EMouseCode Button, EInputState State) {
  mMouseButtons[Button] = State;

  switch (State) {
  case EInputState::DOWN:
    for (const auto &Func : mMouseButtonPressCallbacks[Button]) {
      Func();
    }
    for (const auto &Func : mGenericMouseButtonPressCallbacks) {
      Func(Button);
    }
    break;
  case EInputState::UP:
    for (const auto &Func : mMouseButtonReleaseCallbacks[Button]) {
      Func();
    }
    for (const auto &Func : mGenericMouseButtonReleaseCallbacks) {
      Func(Button);
    }
    break;
  }

  for (const auto &Func : mGenericMouseButtonCallbacks) {
    Func(State, Button);
  }
}

void FInput::SetMousePos(FFloat X, FFloat Y) {
  mMousePos = { X, Y };

  for (const auto &Func : mMouseMoveCallbacks) {
    Func(X, Y);
  }
}

#ifdef SK_GLFW
#include "GLFWInput.h"
#define PlatformEnableRawInput GLFWEnableRawInput
#define PlatformDisableRawInput GLFWEnableRawInput
#define PlatformSetCursorVisibility GLFWSetCursorVisibility
#endif

void FInput::SetRawInput(FBool Value) {
  if (mIsUsingRawInput != Value) {
    mIsUsingRawInput = Value;
    if (Value) {
      if (mCursorVisibility == ECursorVisibility::DISABLED) {
        PlatformEnableRawInput();
      }
      else {
        SK_LOG_ERROR("Cannot enable raw input while cursor is not disabled");
      }
    }
    else {
      PlatformDisableRawInput();
    }
  }
}

void FInput::SetCursorVisibility(ECursorVisibility Visibility) {
  if (Visibility != mCursorVisibility) {
    mCursorVisibility = Visibility;
    if (mCursorVisibility != ECursorVisibility::DISABLED && mIsUsingRawInput) {
      SK_LOG_ERROR("Cannot change cursor state from disabled while raw input is enabled");
      return;
    }
    PlatformSetCursorVisibility(Visibility);
  }
}
