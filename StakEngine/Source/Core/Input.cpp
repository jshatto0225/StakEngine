#include "Input.h"

#include "Log.h"
#include "Window.h"
#undef DELETE

FInput::FInput(TRef<IWindow> Win) {
    Window = Win;

    Keys[EKeyCode::UNKNOWN] = EInputState::UP;
    Keys[EKeyCode::SPACE] = EInputState::UP;
    Keys[EKeyCode::APOSTROPHE] = EInputState::UP;
    Keys[EKeyCode::COMMA] = EInputState::UP;
    Keys[EKeyCode::MINUS] = EInputState::UP;
    Keys[EKeyCode::PERIOD] = EInputState::UP;
    Keys[EKeyCode::SLASH] = EInputState::UP;
    Keys[EKeyCode::ZERO] = EInputState::UP;
    Keys[EKeyCode::ONE] = EInputState::UP;
    Keys[EKeyCode::TWO] = EInputState::UP;
    Keys[EKeyCode::THREE] = EInputState::UP;
    Keys[EKeyCode::FOUR] = EInputState::UP;
    Keys[EKeyCode::FIVE] = EInputState::UP;
    Keys[EKeyCode::SIX] = EInputState::UP;
    Keys[EKeyCode::SEVEN] = EInputState::UP;
    Keys[EKeyCode::EIGHT] = EInputState::UP;
    Keys[EKeyCode::NINE] = EInputState::UP;
    Keys[EKeyCode::SEMICOLON] = EInputState::UP;
    Keys[EKeyCode::EQUAL] = EInputState::UP;
    Keys[EKeyCode::A] = EInputState::UP;
    Keys[EKeyCode::B] = EInputState::UP;
    Keys[EKeyCode::C] = EInputState::UP;
    Keys[EKeyCode::D] = EInputState::UP;
    Keys[EKeyCode::E] = EInputState::UP;
    Keys[EKeyCode::F] = EInputState::UP;
    Keys[EKeyCode::G] = EInputState::UP;
    Keys[EKeyCode::H] = EInputState::UP;
    Keys[EKeyCode::I] = EInputState::UP;
    Keys[EKeyCode::J] = EInputState::UP;
    Keys[EKeyCode::K] = EInputState::UP;
    Keys[EKeyCode::L] = EInputState::UP;
    Keys[EKeyCode::M] = EInputState::UP;
    Keys[EKeyCode::N] = EInputState::UP;
    Keys[EKeyCode::O] = EInputState::UP;
    Keys[EKeyCode::P] = EInputState::UP;
    Keys[EKeyCode::Q] = EInputState::UP;
    Keys[EKeyCode::R] = EInputState::UP;
    Keys[EKeyCode::S] = EInputState::UP;
    Keys[EKeyCode::T] = EInputState::UP;
    Keys[EKeyCode::U] = EInputState::UP;
    Keys[EKeyCode::V] = EInputState::UP;
    Keys[EKeyCode::W] = EInputState::UP;
    Keys[EKeyCode::X] = EInputState::UP;
    Keys[EKeyCode::Y] = EInputState::UP;
    Keys[EKeyCode::Z] = EInputState::UP;
    Keys[EKeyCode::LEFT_BRACKET] = EInputState::UP;
    Keys[EKeyCode::BACKSLASH] = EInputState::UP;
    Keys[EKeyCode::RIGHT_BRACKET] = EInputState::UP;
    Keys[EKeyCode::GRAVE_ACCENT] = EInputState::UP;
    Keys[EKeyCode::WORLD_1] = EInputState::UP;
    Keys[EKeyCode::WORLD_2] = EInputState::UP;
    Keys[EKeyCode::ESCAPE] = EInputState::UP;
    Keys[EKeyCode::ENTER] = EInputState::UP;
    Keys[EKeyCode::TAB] = EInputState::UP;
    Keys[EKeyCode::BACKSPACE] = EInputState::UP;
    Keys[EKeyCode::INSERT] = EInputState::UP;
    Keys[EKeyCode::DELETE] = EInputState::UP;
    Keys[EKeyCode::RIGHT] = EInputState::UP;
    Keys[EKeyCode::LEFT] = EInputState::UP;
    Keys[EKeyCode::DOWN] = EInputState::UP;
    Keys[EKeyCode::UP] = EInputState::UP;
    Keys[EKeyCode::PAGE_UP] = EInputState::UP;
    Keys[EKeyCode::PAGE_DOWN] = EInputState::UP;
    Keys[EKeyCode::HOME] = EInputState::UP;
    Keys[EKeyCode::END] = EInputState::UP;
    Keys[EKeyCode::CAPS_LOCK] = EInputState::UP;
    Keys[EKeyCode::SCROLL_LOCK] = EInputState::UP;
    Keys[EKeyCode::NUM_LOCK] = EInputState::UP;
    Keys[EKeyCode::PRINT_SCREEN] = EInputState::UP;
    Keys[EKeyCode::PAUSE] = EInputState::UP;
    Keys[EKeyCode::F1] = EInputState::UP;
    Keys[EKeyCode::F2] = EInputState::UP;
    Keys[EKeyCode::F3] = EInputState::UP;
    Keys[EKeyCode::F4] = EInputState::UP;
    Keys[EKeyCode::F5] = EInputState::UP;
    Keys[EKeyCode::F6] = EInputState::UP;
    Keys[EKeyCode::F7] = EInputState::UP;
    Keys[EKeyCode::F8] = EInputState::UP;
    Keys[EKeyCode::F9] = EInputState::UP;
    Keys[EKeyCode::F10] = EInputState::UP;
    Keys[EKeyCode::F11] = EInputState::UP;
    Keys[EKeyCode::F12] = EInputState::UP;
    Keys[EKeyCode::F13] = EInputState::UP;
    Keys[EKeyCode::F14] = EInputState::UP;
    Keys[EKeyCode::F15] = EInputState::UP;
    Keys[EKeyCode::F16] = EInputState::UP;
    Keys[EKeyCode::F17] = EInputState::UP;
    Keys[EKeyCode::F18] = EInputState::UP;
    Keys[EKeyCode::F19] = EInputState::UP;
    Keys[EKeyCode::F20] = EInputState::UP;
    Keys[EKeyCode::F21] = EInputState::UP;
    Keys[EKeyCode::F22] = EInputState::UP;
    Keys[EKeyCode::F23] = EInputState::UP;
    Keys[EKeyCode::F24] = EInputState::UP;
    Keys[EKeyCode::F25] = EInputState::UP;
    Keys[EKeyCode::KP_0] = EInputState::UP;
    Keys[EKeyCode::KP_1] = EInputState::UP;
    Keys[EKeyCode::KP_2] = EInputState::UP;
    Keys[EKeyCode::KP_3] = EInputState::UP;
    Keys[EKeyCode::KP_4] = EInputState::UP;
    Keys[EKeyCode::KP_5] = EInputState::UP;
    Keys[EKeyCode::KP_6] = EInputState::UP;
    Keys[EKeyCode::KP_7] = EInputState::UP;
    Keys[EKeyCode::KP_8] = EInputState::UP;
    Keys[EKeyCode::KP_9] = EInputState::UP;
    Keys[EKeyCode::KP_DECIMAL] = EInputState::UP;
    Keys[EKeyCode::KP_DIVIDE] = EInputState::UP;
    Keys[EKeyCode::KP_MULTIPLY] = EInputState::UP;
    Keys[EKeyCode::KP_SUBTRACT] = EInputState::UP;
    Keys[EKeyCode::KP_ADD] = EInputState::UP;
    Keys[EKeyCode::KP_ENTER] = EInputState::UP;
    Keys[EKeyCode::KP_EQUAL] = EInputState::UP;
    Keys[EKeyCode::LEFT_SHIFT] = EInputState::UP;
    Keys[EKeyCode::LEFT_CONTROL] = EInputState::UP;
    Keys[EKeyCode::LEFT_ALT] = EInputState::UP;
    Keys[EKeyCode::LEFT_SUPER] = EInputState::UP;
    Keys[EKeyCode::RIGHT_SHIFT] = EInputState::UP;
    Keys[EKeyCode::RIGHT_CONTROL] = EInputState::UP;
    Keys[EKeyCode::RIGHT_ALT] = EInputState::UP;
    Keys[EKeyCode::RIGHT_SUPER] = EInputState::UP;
    Keys[EKeyCode::MENU] = EInputState::UP;
    Keys[EKeyCode::LAST] = EInputState::UP;

    MouseButtons[EMouseCode::ONE] = EInputState::UP;
    MouseButtons[EMouseCode::TWO] = EInputState::UP;
    MouseButtons[EMouseCode::THREE] = EInputState::UP;
    MouseButtons[EMouseCode::FOUR] = EInputState::UP;
    MouseButtons[EMouseCode::FIVE] = EInputState::UP;
    MouseButtons[EMouseCode::SIX] = EInputState::UP;
    MouseButtons[EMouseCode::SEVEN] = EInputState::UP;
    MouseButtons[EMouseCode::EIGHT] = EInputState::UP;
    MouseButtons[EMouseCode::LAST] = EInputState::UP;
    MouseButtons[EMouseCode::LEFT] = EInputState::UP;
    MouseButtons[EMouseCode::RIGHT] = EInputState::UP;
    MouseButtons[EMouseCode::MIDDLE] = EInputState::UP;

    UsingRawInput = false;
    CursorVisibility = ECursorVisibility::NORMAL;
}

void FInput::AddKeyStateCallback(EInputState State, EKeyCode Key, std::function<void()> Func) {
    switch (State) {
    case EInputState::DOWN:
        KeyPressCallbacks[Key].push_back(Func);
        break;
    case EInputState::UP:
        KeyReleaseCallbacks[Key].push_back(Func);
        break;
    }
}

void FInput::AddGenericKeyStateCallback(EInputState State, std::function<void(EKeyCode)> Func) {
    switch (State) {
    case EInputState::DOWN:
        GenericKeyPressCallbacks.push_back(Func);
        break;
    case EInputState::UP:
        GenericKeyReleaseCallbacks.push_back(Func);
        break;
    }
}

void FInput::AddGenericKeyCallback(std::function<void(EInputState, EKeyCode)> Func) {
    GenericKeyCallbacks.push_back(Func);
}

void FInput::AddMouseButtonStateCallback(EInputState State, EMouseCode Button, std::function<void()> Func) {
    switch (State) {
    case EInputState::DOWN:
        MouseButtonPressCallbacks[Button].push_back(Func);
        break;
    case EInputState::UP:
        MouseButtonReleaseCallbacks[Button].push_back(Func);
        break;
    }
}

void FInput::AddGenericMouseButtonStateCallback(EInputState State, std::function<void(EMouseCode)> Func) {
    switch (State) {
    case EInputState::DOWN:
        GenericMouseButtonPressCallbacks.push_back(Func);
        break;
    case EInputState::UP:
        GenericMouseButtonReleaseCallbacks.push_back(Func);
        break;
    }
}

void FInput::AddGenericMouseButtonCallback(std::function<void(EInputState, EMouseCode)> Func) {
    GenericMouseButtonCallbacks.push_back(Func);
}

void FInput::AddMouseMoveCallback(std::function<void(FFloat, FFloat)> Func) {
    MouseMoveCallbacks.push_back(Func);
}

void FInput::SetKey(EKeyCode Key, EInputState State) {
    Keys[Key] = State;

    switch (State) {
    case EInputState::DOWN:
        for (const auto &Func : KeyPressCallbacks[Key]) {
            Func();
        }
        for (const auto &Func : GenericKeyPressCallbacks) {
            Func(Key);
        }
        break;
    case EInputState::UP:
        for (const auto &Func : KeyReleaseCallbacks[Key]) {
            Func();
        }
        for (const auto &Func : GenericKeyReleaseCallbacks) {
            Func(Key);
        }
        break;
    }

    for (const auto &Func : GenericKeyCallbacks) {
        Func(State, Key);
    }
}

void FInput::SetMouseButton(EMouseCode Button, EInputState State) {
    MouseButtons[Button] = State;

    switch (State) {
    case EInputState::DOWN:
        for (const auto &Func : MouseButtonPressCallbacks[Button]) {
            Func();
        }
        for (const auto &Func : GenericMouseButtonPressCallbacks) {
            Func(Button);
        }
        break;
    case EInputState::UP:
        for (const auto &Func : MouseButtonReleaseCallbacks[Button]) {
            Func();
        }
        for (const auto &Func : GenericMouseButtonReleaseCallbacks) {
            Func(Button);
        }
        break;
    }

    for (const auto &Func : GenericMouseButtonCallbacks) {
        Func(State, Button);
    }
}

void FInput::SetMousePos(FFloat X, FFloat Y) {
    MousePos = { X, Y };

    for (const auto &Func : MouseMoveCallbacks) {
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
    if (UsingRawInput != Value) {
        UsingRawInput = Value;
        if (Value) {
            if (CursorVisibility == ECursorVisibility::DISABLED) {
                PlatformEnableRawInput(Window);
            }
            else {
                SK_LOG_ERROR("Cannot enable raw input while cursor is not disabled");
            }
        }
        else {
            PlatformDisableRawInput(Window);
        }
    }
}

void FInput::SetCursorVisibility(ECursorVisibility Visibility) {
    if (Visibility != CursorVisibility) {
        CursorVisibility = Visibility;
        if (CursorVisibility != ECursorVisibility::DISABLED && UsingRawInput) {
            SK_LOG_ERROR("Cannot change cursor state from disabled while raw input is enabled");
            return;
        }
        PlatformSetCursorVisibility(Window, Visibility);
    }
}
