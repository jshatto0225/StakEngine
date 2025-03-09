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

FInputCallbackInfo FInput::AddKeyPressCallback(EKeyCode Key, std::function<void()> Func) {
    KeyPressCallbacks[Key][KeyPressId] = Func;

    FInputCallbackInfo Info = {};

    Info.Type = ECallbackType::KEY;
    Info.Key = Key;
    Info.Action = EInputState::DOWN;
    Info.Id = KeyPressId;

    KeyPressId++;

    return Info;
}

FInputCallbackInfo FInput::AddKeyReleaseCallback(EKeyCode Key, std::function<void()> Func) {
    KeyReleaseCallbacks[Key][KeyReleaseId] = Func;

    FInputCallbackInfo Info = {};

    Info.Type = ECallbackType::KEY;
    Info.Key = Key;
    Info.Action = EInputState::UP;
    Info.Id = KeyReleaseId;

    KeyReleaseId++;

    return Info;
}

FInputCallbackInfo FInput::AddAnyKeyPressCallback(std::function<void(EKeyCode)> Func) {
    AnyKeyPressCallbacks[AnyKeyPressId] = Func;

    FInputCallbackInfo Info = {};

    Info.Type = ECallbackType::KEY;
    Info.Action = EInputState::DOWN;
    Info.IsAnyCode = true;
    Info.Id = AnyKeyPressId;

    AnyKeyPressId++;

    return Info;
}

FInputCallbackInfo FInput::AddAnyKeyReleaseCallback(std::function<void(EKeyCode)> Func) {
    AnyKeyReleaseCallbacks[AnyKeyReleaseId] = Func;

    FInputCallbackInfo Info = {};

    Info.Type = ECallbackType::KEY;
    Info.Action = EInputState::UP;
    Info.IsAnyCode = true;
    Info.Id = AnyKeyReleaseId;

    AnyKeyReleaseId++;

    return Info;
}

FInputCallbackInfo FInput::AddKeyAnyActionCallback(EKeyCode Key, std::function<void(EInputState)> Func) {
    KeyAnyActionCallbacks[Key][KeyAnyActionId] = Func;

    FInputCallbackInfo Info = {};

    Info.Type = ECallbackType::KEY;
    Info.IsAnyCode = true;
    Info.Key = Key;
    Info.Id = KeyAnyActionId;

    KeyAnyActionId++;

    return Info;
}

FInputCallbackInfo FInput::AddAnyKeyAnyActionCallback(std::function<void(EInputState, EKeyCode)> Func) {
    AnyKeyAnyActionCallbacks[AnyKeyAnyActionId] = Func;

    FInputCallbackInfo Info = {};

    Info.Type = ECallbackType::KEY;
    Info.IsAnyCode = true;
    Info.IsAnyAction = true;
    Info.Id = AnyKeyAnyActionId;

    AnyKeyAnyActionId++;

    return Info;
}

FInputCallbackInfo FInput::AddMouseButtonPressCallback(EMouseCode Button, std::function<void()> Func) {
    MouseButtonPressCallbacks[Button][MouseButtonPressId] = Func;

    FInputCallbackInfo Info = {};

    Info.Type = ECallbackType::MOUSE_BUTTON;
    Info.Button = Button;
    Info.Action = EInputState::DOWN;
    Info.Id = MouseButtonPressId;

    MouseButtonPressId++;

    return Info;
}

FInputCallbackInfo FInput::AddMouseButtonReleaseCallback(EMouseCode Button, std::function<void()> Func) {
    MouseButtonReleaseCallbacks[Button][MouseButtonReleaseId] = Func;

    FInputCallbackInfo Info = {};

    Info.Type = ECallbackType::MOUSE_BUTTON;
    Info.Button = Button;
    Info.Action = EInputState::UP;
    Info.Id = MouseButtonReleaseId;

    MouseButtonReleaseId++;

    return Info;
}

FInputCallbackInfo FInput::AddAnyMouseButtonPressCallback(std::function<void(EMouseCode)> Func) {
    AnyMouseButtonPressCallbacks[AnyMouseButtonPressId] = Func;

    FInputCallbackInfo Info = {};

    Info.Type = ECallbackType::MOUSE_BUTTON;
    Info.Action = EInputState::DOWN;
    Info.IsAnyCode = true;
    Info.Id = AnyMouseButtonPressId;

    AnyMouseButtonPressId++;

    return Info;
}

FInputCallbackInfo FInput::AddAnyMouseButtonReleaseCallback(std::function<void(EMouseCode)> Func) {
    AnyMouseButtonReleaseCallbacks[AnyMouseButtonReleaseId] = Func;

    FInputCallbackInfo Info = {};

    Info.Type = ECallbackType::MOUSE_BUTTON;
    Info.Action = EInputState::UP;
    Info.IsAnyCode = true;
    Info.Id = AnyMouseButtonReleaseId;

    AnyMouseButtonReleaseId++;

    return Info;
}

FInputCallbackInfo FInput::AddMouseButtonAnyActionCallback(EMouseCode Button, std::function<void(EInputState)> Func) {
    MouseButtonAnyActionCallbacks[Button][MouseButtonAnyActionId] = Func;

    FInputCallbackInfo Info = {};

    Info.Type = ECallbackType::MOUSE_BUTTON;
    Info.IsAnyAction = true;
    Info.Button = Button;
    Info.Id = MouseButtonAnyActionId;

    MouseButtonAnyActionId++;

    return Info;
}

FInputCallbackInfo FInput::AddAnyMouseButtonAnyActionCallback(std::function<void(EInputState, EMouseCode)> Func) {
    AnyMouseButtonAnyActionCallbacks[AnyMouseButtonAnyActionId] = Func;

    FInputCallbackInfo Info = {};

    Info.Type = ECallbackType::MOUSE_BUTTON;
    Info.IsAnyAction = true;
    Info.IsAnyCode = true;
    Info.Id = AnyMouseButtonAnyActionId;

    AnyMouseButtonAnyActionId++;

    return Info;
}

FInputCallbackInfo FInput::AddMouseMoveCallback(std::function<void(FFloat, FFloat)> Func) {
    MouseMoveCallbacks[MouseMoveId] = Func;

    FInputCallbackInfo Info = {};

    Info.Type = ECallbackType::MOUSE_MOVE;
    Info.Id = MouseMoveId;

    MouseMoveId++;

    return Info;
}

void FInput::SetKey(EKeyCode Key, EInputState State) {
    Keys[Key] = State;

    switch (State) {
    case EInputState::DOWN:
        for (auto &It : KeyPressCallbacks[Key]) {
            It.second();
        }
        for (auto &It : AnyKeyPressCallbacks) {
            It.second(Key);
        }
        break;
    case EInputState::UP:
        for (auto &It : KeyReleaseCallbacks[Key]) {
            It.second();
        }
        for (auto &It : AnyKeyReleaseCallbacks) {
            It.second(Key);
        }
        break;
    }

    for (auto &It : KeyAnyActionCallbacks[Key]) {
        It.second(State);
    }
    for (auto &It : AnyKeyAnyActionCallbacks) {
        It.second(State, Key);
    }
}

void FInput::SetMouseButton(EMouseCode Button, EInputState State) {
    MouseButtons[Button] = State;

    switch (State) {
    case EInputState::DOWN:
        for (auto &It : MouseButtonPressCallbacks[Button]) {
            It.second();
        }
        for (auto &It : AnyMouseButtonPressCallbacks) {
            It.second(Button);
        }
        break;
    case EInputState::UP:
        for (auto &It : MouseButtonReleaseCallbacks[Button]) {
            It.second();
        }
        for (auto &It : AnyMouseButtonReleaseCallbacks) {
            It.second(Button);
        }
        break;
    }

    for (auto &It : MouseButtonAnyActionCallbacks[Button]) {
        It.second(State);
    }
    for (const auto &It : AnyMouseButtonAnyActionCallbacks) {
        It.second(State, Button);
    }
}

void FInput::SetMousePos(FFloat X, FFloat Y) {
    MousePos = { X, Y };

    for (auto &It : MouseMoveCallbacks) {
        It.second(X, Y);
    }
}

void FInput::RemoveCallback(const FInputCallbackInfo &Info) {
    switch (Info.Type) {
    case ECallbackType::KEY:
        if (Info.IsAnyAction && Info.IsAnyCode) {
            AnyKeyAnyActionCallbacks.erase(Info.Id);
        } else if (Info.IsAnyAction) {
            KeyAnyActionCallbacks[Info.Key].erase(Info.Id);
        } else if (Info.IsAnyCode) {
            switch (Info.Action) {
            case EInputState::UP:
                AnyKeyReleaseCallbacks.erase(Info.Id);
                break;
            case EInputState::DOWN:
                AnyKeyPressCallbacks.erase(Info.Id);
                break;
            }
        } else {
            switch (Info.Action) {
            case EInputState::UP:
                KeyReleaseCallbacks[Info.Key].erase(Info.Id);
                break;
            case EInputState::DOWN:
                KeyPressCallbacks[Info.Key].erase(Info.Id);
                break;
            }
        }
        break;
    case ECallbackType::MOUSE_BUTTON:
        if (Info.IsAnyAction && Info.IsAnyCode) {
            AnyMouseButtonAnyActionCallbacks.erase(Info.Id);
        } else if (Info.IsAnyAction) {
            MouseButtonAnyActionCallbacks[Info.Button].erase(Info.Id);
        } else if (Info.IsAnyCode) {
            switch (Info.Action) {
            case EInputState::UP:
                AnyMouseButtonReleaseCallbacks.erase(Info.Id);
                break;
            case EInputState::DOWN:
                AnyMouseButtonPressCallbacks.erase(Info.Id);
                break;
            }
        } else {
            switch (Info.Action) {
            case EInputState::UP:
                MouseButtonReleaseCallbacks[Info.Button].erase(Info.Id);
                break;
            case EInputState::DOWN:
                MouseButtonPressCallbacks[Info.Button].erase(Info.Id);
                break;
            }
        }
        break;
    case ECallbackType::MOUSE_MOVE:
        MouseMoveCallbacks.erase(Info.Id);
        break;
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
