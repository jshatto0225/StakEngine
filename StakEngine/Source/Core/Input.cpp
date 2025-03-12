#include "Input.h"

#include "Log.h"
#include "Window.h"
#include "Platform.h"
#undef DELETE


bool InitializeInput(FInput *Input, FWindow *Window) {
    Input->Window = Window;

    Input->Keyboard[EKeyCode::UNKNOWN] = EInputState::UP;
    Input->Keyboard[EKeyCode::SPACE] = EInputState::UP;
    Input->Keyboard[EKeyCode::APOSTROPHE] = EInputState::UP;
    Input->Keyboard[EKeyCode::COMMA] = EInputState::UP;
    Input->Keyboard[EKeyCode::MINUS] = EInputState::UP;
    Input->Keyboard[EKeyCode::PERIOD] = EInputState::UP;
    Input->Keyboard[EKeyCode::SLASH] = EInputState::UP;
    Input->Keyboard[EKeyCode::ZERO] = EInputState::UP;
    Input->Keyboard[EKeyCode::ONE] = EInputState::UP;
    Input->Keyboard[EKeyCode::TWO] = EInputState::UP;
    Input->Keyboard[EKeyCode::THREE] = EInputState::UP;
    Input->Keyboard[EKeyCode::FOUR] = EInputState::UP;
    Input->Keyboard[EKeyCode::FIVE] = EInputState::UP;
    Input->Keyboard[EKeyCode::SIX] = EInputState::UP;
    Input->Keyboard[EKeyCode::SEVEN] = EInputState::UP;
    Input->Keyboard[EKeyCode::EIGHT] = EInputState::UP;
    Input->Keyboard[EKeyCode::NINE] = EInputState::UP;
    Input->Keyboard[EKeyCode::SEMICOLON] = EInputState::UP;
    Input->Keyboard[EKeyCode::EQUAL] = EInputState::UP;
    Input->Keyboard[EKeyCode::A] = EInputState::UP;
    Input->Keyboard[EKeyCode::B] = EInputState::UP;
    Input->Keyboard[EKeyCode::C] = EInputState::UP;
    Input->Keyboard[EKeyCode::D] = EInputState::UP;
    Input->Keyboard[EKeyCode::E] = EInputState::UP;
    Input->Keyboard[EKeyCode::F] = EInputState::UP;
    Input->Keyboard[EKeyCode::G] = EInputState::UP;
    Input->Keyboard[EKeyCode::H] = EInputState::UP;
    Input->Keyboard[EKeyCode::I] = EInputState::UP;
    Input->Keyboard[EKeyCode::J] = EInputState::UP;
    Input->Keyboard[EKeyCode::K] = EInputState::UP;
    Input->Keyboard[EKeyCode::L] = EInputState::UP;
    Input->Keyboard[EKeyCode::M] = EInputState::UP;
    Input->Keyboard[EKeyCode::N] = EInputState::UP;
    Input->Keyboard[EKeyCode::O] = EInputState::UP;
    Input->Keyboard[EKeyCode::P] = EInputState::UP;
    Input->Keyboard[EKeyCode::Q] = EInputState::UP;
    Input->Keyboard[EKeyCode::R] = EInputState::UP;
    Input->Keyboard[EKeyCode::S] = EInputState::UP;
    Input->Keyboard[EKeyCode::T] = EInputState::UP;
    Input->Keyboard[EKeyCode::U] = EInputState::UP;
    Input->Keyboard[EKeyCode::V] = EInputState::UP;
    Input->Keyboard[EKeyCode::W] = EInputState::UP;
    Input->Keyboard[EKeyCode::X] = EInputState::UP;
    Input->Keyboard[EKeyCode::Y] = EInputState::UP;
    Input->Keyboard[EKeyCode::Z] = EInputState::UP;
    Input->Keyboard[EKeyCode::LEFT_BRACKET] = EInputState::UP;
    Input->Keyboard[EKeyCode::BACKSLASH] = EInputState::UP;
    Input->Keyboard[EKeyCode::RIGHT_BRACKET] = EInputState::UP;
    Input->Keyboard[EKeyCode::GRAVE_ACCENT] = EInputState::UP;
    Input->Keyboard[EKeyCode::WORLD_1] = EInputState::UP;
    Input->Keyboard[EKeyCode::WORLD_2] = EInputState::UP;
    Input->Keyboard[EKeyCode::ESCAPE] = EInputState::UP;
    Input->Keyboard[EKeyCode::ENTER] = EInputState::UP;
    Input->Keyboard[EKeyCode::TAB] = EInputState::UP;
    Input->Keyboard[EKeyCode::BACKSPACE] = EInputState::UP;
    Input->Keyboard[EKeyCode::INSERT] = EInputState::UP;
    Input->Keyboard[EKeyCode::DELETE] = EInputState::UP;
    Input->Keyboard[EKeyCode::RIGHT] = EInputState::UP;
    Input->Keyboard[EKeyCode::LEFT] = EInputState::UP;
    Input->Keyboard[EKeyCode::DOWN] = EInputState::UP;
    Input->Keyboard[EKeyCode::UP] = EInputState::UP;
    Input->Keyboard[EKeyCode::PAGE_UP] = EInputState::UP;
    Input->Keyboard[EKeyCode::PAGE_DOWN] = EInputState::UP;
    Input->Keyboard[EKeyCode::HOME] = EInputState::UP;
    Input->Keyboard[EKeyCode::END] = EInputState::UP;
    Input->Keyboard[EKeyCode::CAPS_LOCK] = EInputState::UP;
    Input->Keyboard[EKeyCode::SCROLL_LOCK] = EInputState::UP;
    Input->Keyboard[EKeyCode::NUM_LOCK] = EInputState::UP;
    Input->Keyboard[EKeyCode::PRINT_SCREEN] = EInputState::UP;
    Input->Keyboard[EKeyCode::PAUSE] = EInputState::UP;
    Input->Keyboard[EKeyCode::F1] = EInputState::UP;
    Input->Keyboard[EKeyCode::F2] = EInputState::UP;
    Input->Keyboard[EKeyCode::F3] = EInputState::UP;
    Input->Keyboard[EKeyCode::F4] = EInputState::UP;
    Input->Keyboard[EKeyCode::F5] = EInputState::UP;
    Input->Keyboard[EKeyCode::F6] = EInputState::UP;
    Input->Keyboard[EKeyCode::F7] = EInputState::UP;
    Input->Keyboard[EKeyCode::F8] = EInputState::UP;
    Input->Keyboard[EKeyCode::F9] = EInputState::UP;
    Input->Keyboard[EKeyCode::F10] = EInputState::UP;
    Input->Keyboard[EKeyCode::F11] = EInputState::UP;
    Input->Keyboard[EKeyCode::F12] = EInputState::UP;
    Input->Keyboard[EKeyCode::F13] = EInputState::UP;
    Input->Keyboard[EKeyCode::F14] = EInputState::UP;
    Input->Keyboard[EKeyCode::F15] = EInputState::UP;
    Input->Keyboard[EKeyCode::F16] = EInputState::UP;
    Input->Keyboard[EKeyCode::F17] = EInputState::UP;
    Input->Keyboard[EKeyCode::F18] = EInputState::UP;
    Input->Keyboard[EKeyCode::F19] = EInputState::UP;
    Input->Keyboard[EKeyCode::F20] = EInputState::UP;
    Input->Keyboard[EKeyCode::F21] = EInputState::UP;
    Input->Keyboard[EKeyCode::F22] = EInputState::UP;
    Input->Keyboard[EKeyCode::F23] = EInputState::UP;
    Input->Keyboard[EKeyCode::F24] = EInputState::UP;
    Input->Keyboard[EKeyCode::F25] = EInputState::UP;
    Input->Keyboard[EKeyCode::KP_0] = EInputState::UP;
    Input->Keyboard[EKeyCode::KP_1] = EInputState::UP;
    Input->Keyboard[EKeyCode::KP_2] = EInputState::UP;
    Input->Keyboard[EKeyCode::KP_3] = EInputState::UP;
    Input->Keyboard[EKeyCode::KP_4] = EInputState::UP;
    Input->Keyboard[EKeyCode::KP_5] = EInputState::UP;
    Input->Keyboard[EKeyCode::KP_6] = EInputState::UP;
    Input->Keyboard[EKeyCode::KP_7] = EInputState::UP;
    Input->Keyboard[EKeyCode::KP_8] = EInputState::UP;
    Input->Keyboard[EKeyCode::KP_9] = EInputState::UP;
    Input->Keyboard[EKeyCode::KP_DECIMAL] = EInputState::UP;
    Input->Keyboard[EKeyCode::KP_DIVIDE] = EInputState::UP;
    Input->Keyboard[EKeyCode::KP_MULTIPLY] = EInputState::UP;
    Input->Keyboard[EKeyCode::KP_SUBTRACT] = EInputState::UP;
    Input->Keyboard[EKeyCode::KP_ADD] = EInputState::UP;
    Input->Keyboard[EKeyCode::KP_ENTER] = EInputState::UP;
    Input->Keyboard[EKeyCode::KP_EQUAL] = EInputState::UP;
    Input->Keyboard[EKeyCode::LEFT_SHIFT] = EInputState::UP;
    Input->Keyboard[EKeyCode::LEFT_CONTROL] = EInputState::UP;
    Input->Keyboard[EKeyCode::LEFT_ALT] = EInputState::UP;
    Input->Keyboard[EKeyCode::LEFT_SUPER] = EInputState::UP;
    Input->Keyboard[EKeyCode::RIGHT_SHIFT] = EInputState::UP;
    Input->Keyboard[EKeyCode::RIGHT_CONTROL] = EInputState::UP;
    Input->Keyboard[EKeyCode::RIGHT_ALT] = EInputState::UP;
    Input->Keyboard[EKeyCode::RIGHT_SUPER] = EInputState::UP;
    Input->Keyboard[EKeyCode::MENU] = EInputState::UP;
    Input->Keyboard[EKeyCode::LAST] = EInputState::UP;

    Input->Mouse.Buttons[EMouseCode::ONE] = EInputState::UP;
    Input->Mouse.Buttons[EMouseCode::TWO] = EInputState::UP;
    Input->Mouse.Buttons[EMouseCode::THREE] = EInputState::UP;
    Input->Mouse.Buttons[EMouseCode::FOUR] = EInputState::UP;
    Input->Mouse.Buttons[EMouseCode::FIVE] = EInputState::UP;
    Input->Mouse.Buttons[EMouseCode::SIX] = EInputState::UP;
    Input->Mouse.Buttons[EMouseCode::SEVEN] = EInputState::UP;
    Input->Mouse.Buttons[EMouseCode::EIGHT] = EInputState::UP;
    Input->Mouse.Buttons[EMouseCode::LAST] = EInputState::UP;
    Input->Mouse.Buttons[EMouseCode::LEFT] = EInputState::UP;
    Input->Mouse.Buttons[EMouseCode::RIGHT] = EInputState::UP;
    Input->Mouse.Buttons[EMouseCode::MIDDLE] = EInputState::UP;

    Input->UsingRawInput = false;
    Input->CursorVisibility = ECursorVisibility::NORMAL;

    return true;
}

FInputCallbackInfo AddKeyPressCallback(FInput *Input, EKeyCode Key, std::function<void()> Func) {
    Input->KeyPressCallbacks[Key][Input->KeyPressId] = Func;

    FInputCallbackInfo Info = {};

    Info.Type = ECallbackType::KEY;
    Info.Key = Key;
    Info.Action = EInputState::DOWN;
    Info.Id = Input->KeyPressId;

    Input->KeyPressId++;

    return Info;
}

FInputCallbackInfo AddKeyReleaseCallback(FInput *Input, EKeyCode Key, std::function<void()> Func) {
    Input->KeyReleaseCallbacks[Key][Input->KeyReleaseId] = Func;

    FInputCallbackInfo Info = {};

    Info.Type = ECallbackType::KEY;
    Info.Key = Key;
    Info.Action = EInputState::UP;
    Info.Id = Input->KeyReleaseId;

    Input->KeyReleaseId++;

    return Info;
}

FInputCallbackInfo AddAnyKeyPressCallback(FInput *Input, std::function<void(EKeyCode)> Func) {
    Input->AnyKeyPressCallbacks[Input->AnyKeyPressId] = Func;

    FInputCallbackInfo Info = {};

    Info.Type = ECallbackType::KEY;
    Info.Action = EInputState::DOWN;
    Info.IsAnyCode = true;
    Info.Id = Input->AnyKeyPressId;

    Input->AnyKeyPressId++;

    return Info;
}

FInputCallbackInfo AddAnyKeyReleaseCallback(FInput *Input, std::function<void(EKeyCode)> Func) {
    Input->AnyKeyReleaseCallbacks[Input->AnyKeyReleaseId] = Func;

    FInputCallbackInfo Info = {};

    Info.Type = ECallbackType::KEY;
    Info.Action = EInputState::UP;
    Info.IsAnyCode = true;
    Info.Id = Input->AnyKeyReleaseId;

    Input->AnyKeyReleaseId++;

    return Info;
}

FInputCallbackInfo AddKeyAnyActionCallback(FInput *Input, EKeyCode Key, std::function<void(EInputState)> Func) {
    Input->KeyAnyActionCallbacks[Key][Input->KeyAnyActionId] = Func;

    FInputCallbackInfo Info = {};

    Info.Type = ECallbackType::KEY;
    Info.IsAnyCode = true;
    Info.Key = Key;
    Info.Id = Input->KeyAnyActionId;

    Input->KeyAnyActionId++;

    return Info;
}

FInputCallbackInfo AddAnyKeyAnyActionCallback(FInput *Input, std::function<void(EInputState, EKeyCode)> Func) {
    Input->AnyKeyAnyActionCallbacks[Input->AnyKeyAnyActionId] = Func;

    FInputCallbackInfo Info = {};

    Info.Type = ECallbackType::KEY;
    Info.IsAnyCode = true;
    Info.IsAnyAction = true;
    Info.Id = Input->AnyKeyAnyActionId;

    Input->AnyKeyAnyActionId++;

    return Info;
}

FInputCallbackInfo AddMouseButtonPressCallback(FInput *Input, EMouseCode Button, std::function<void()> Func) {
    Input->MouseButtonPressCallbacks[Button][Input->MouseButtonPressId] = Func;

    FInputCallbackInfo Info = {};

    Info.Type = ECallbackType::MOUSE_BUTTON;
    Info.Button = Button;
    Info.Action = EInputState::DOWN;
    Info.Id = Input->MouseButtonPressId;

    Input->MouseButtonPressId++;

    return Info;
}

FInputCallbackInfo AddMouseButtonReleaseCallback(FInput *Input, EMouseCode Button, std::function<void()> Func) {
    Input->MouseButtonReleaseCallbacks[Button][Input->MouseButtonReleaseId] = Func;

    FInputCallbackInfo Info = {};

    Info.Type = ECallbackType::MOUSE_BUTTON;
    Info.Button = Button;
    Info.Action = EInputState::UP;
    Info.Id = Input->MouseButtonReleaseId;

    Input->MouseButtonReleaseId++;

    return Info;
}

FInputCallbackInfo AddAnyMouseButtonPressCallback(FInput *Input, std::function<void(EMouseCode)> Func) {
    Input->AnyMouseButtonPressCallbacks[Input->AnyMouseButtonPressId] = Func;

    FInputCallbackInfo Info = {};

    Info.Type = ECallbackType::MOUSE_BUTTON;
    Info.Action = EInputState::DOWN;
    Info.IsAnyCode = true;
    Info.Id = Input->AnyMouseButtonPressId;

    Input->AnyMouseButtonPressId++;

    return Info;
}

FInputCallbackInfo AddAnyMouseButtonReleaseCallback(FInput *Input, std::function<void(EMouseCode)> Func) {
    Input->AnyMouseButtonReleaseCallbacks[Input->AnyMouseButtonReleaseId] = Func;

    FInputCallbackInfo Info = {};

    Info.Type = ECallbackType::MOUSE_BUTTON;
    Info.Action = EInputState::UP;
    Info.IsAnyCode = true;
    Info.Id = Input->AnyMouseButtonReleaseId;

    Input->AnyMouseButtonReleaseId++;

    return Info;
}

FInputCallbackInfo AddMouseButtonAnyActionCallback(FInput *Input, EMouseCode Button, std::function<void(EInputState)> Func) {
    Input->MouseButtonAnyActionCallbacks[Button][Input->MouseButtonAnyActionId] = Func;

    FInputCallbackInfo Info = {};

    Info.Type = ECallbackType::MOUSE_BUTTON;
    Info.IsAnyAction = true;
    Info.Button = Button;
    Info.Id = Input->MouseButtonAnyActionId;

    Input->MouseButtonAnyActionId++;

    return Info;
}

FInputCallbackInfo AddAnyMouseButtonAnyActionCallback(FInput *Input, std::function<void(EInputState, EMouseCode)> Func) {
    Input->AnyMouseButtonAnyActionCallbacks[Input->AnyMouseButtonAnyActionId] = Func;

    FInputCallbackInfo Info = {};

    Info.Type = ECallbackType::MOUSE_BUTTON;
    Info.IsAnyAction = true;
    Info.IsAnyCode = true;
    Info.Id = Input->AnyMouseButtonAnyActionId;

    Input->AnyMouseButtonAnyActionId++;

    return Info;
}

FInputCallbackInfo AddMouseMoveCallback(FInput *Input, std::function<void(FFloat, FFloat)> Func) {
    Input->MouseMoveCallbacks[Input->MouseMoveId] = Func;

    FInputCallbackInfo Info = {};

    Info.Type = ECallbackType::MOUSE_MOVE;
    Info.Id = Input->MouseMoveId;

    Input->MouseMoveId++;

    return Info;
}

void SetKey(FInput *Input, EKeyCode Key, EInputState State) {
    Input->Keyboard[Key] = State;

    switch (State) {
    case EInputState::DOWN:
        for (auto &It : Input->KeyPressCallbacks[Key]) {
            It.second();
        }
        for (auto &It : Input->AnyKeyPressCallbacks) {
            It.second(Key);
        }
        break;
    case EInputState::UP:
        for (auto &It : Input->KeyReleaseCallbacks[Key]) {
            It.second();
        }
        for (auto &It : Input->AnyKeyReleaseCallbacks) {
            It.second(Key);
        }
        break;
    }

    for (auto &It : Input->KeyAnyActionCallbacks[Key]) {
        It.second(State);
    }
    for (auto &It : Input->AnyKeyAnyActionCallbacks) {
        It.second(State, Key);
    }
}

void SetMouseButton(FInput *Input, EMouseCode Button, EInputState State) {
    Input->Mouse.Buttons[Button] = State;

    switch (State) {
    case EInputState::DOWN:
        for (auto &It : Input->MouseButtonPressCallbacks[Button]) {
            It.second();
        }
        for (auto &It : Input->AnyMouseButtonPressCallbacks) {
            It.second(Button);
        }
        break;
    case EInputState::UP:
        for (auto &It : Input->MouseButtonReleaseCallbacks[Button]) {
            It.second();
        }
        for (auto &It : Input->AnyMouseButtonReleaseCallbacks) {
            It.second(Button);
        }
        break;
    }

    for (auto &It : Input->MouseButtonAnyActionCallbacks[Button]) {
        It.second(State);
    }
    for (const auto &It : Input->AnyMouseButtonAnyActionCallbacks) {
        It.second(State, Button);
    }
}

void SetMousePos(FInput *Input, FFloat X, FFloat Y) {
    Input->Mouse.X = X;
    Input->Mouse.Y = Y;

    for (auto &It : Input->MouseMoveCallbacks) {
        It.second(X, Y);
    }
}

void RemoveCallback(FInput *Input, const FInputCallbackInfo &Info) {
    switch (Info.Type) {
    case ECallbackType::KEY:
        if (Info.IsAnyAction && Info.IsAnyCode) {
            Input->AnyKeyAnyActionCallbacks.erase(Info.Id);
        } else if (Info.IsAnyAction) {
            Input->KeyAnyActionCallbacks[Info.Key].erase(Info.Id);
        } else if (Info.IsAnyCode) {
            switch (Info.Action) {
            case EInputState::UP:
                Input->AnyKeyReleaseCallbacks.erase(Info.Id);
                break;
            case EInputState::DOWN:
                Input->AnyKeyPressCallbacks.erase(Info.Id);
                break;
            }
        } else {
            switch (Info.Action) {
            case EInputState::UP:
                Input->KeyReleaseCallbacks[Info.Key].erase(Info.Id);
                break;
            case EInputState::DOWN:
                Input->KeyPressCallbacks[Info.Key].erase(Info.Id);
                break;
            }
        }
        break;
    case ECallbackType::MOUSE_BUTTON:
        if (Info.IsAnyAction && Info.IsAnyCode) {
            Input->AnyMouseButtonAnyActionCallbacks.erase(Info.Id);
        } else if (Info.IsAnyAction) {
            Input->MouseButtonAnyActionCallbacks[Info.Button].erase(Info.Id);
        } else if (Info.IsAnyCode) {
            switch (Info.Action) {
            case EInputState::UP:
                Input->AnyMouseButtonReleaseCallbacks.erase(Info.Id);
                break;
            case EInputState::DOWN:
                Input->AnyMouseButtonPressCallbacks.erase(Info.Id);
                break;
            }
        } else {
            switch (Info.Action) {
            case EInputState::UP:
                Input->MouseButtonReleaseCallbacks[Info.Button].erase(Info.Id);
                break;
            case EInputState::DOWN:
                Input->MouseButtonPressCallbacks[Info.Button].erase(Info.Id);
                break;
            }
        }
        break;
    case ECallbackType::MOUSE_MOVE:
        Input->MouseMoveCallbacks.erase(Info.Id);
        break;
    }
}

void SetRawInput(FInput *Input, FBool Value) {
    if (Input->UsingRawInput != Value) {
        Input->UsingRawInput = Value;
        if (Value) {
            if (Input->CursorVisibility == ECursorVisibility::DISABLED) {
                PlatformEnableRawInput(Input->Window);
            }
            else {
                SK_LOG_ERROR("Cannot enable raw input while cursor is not disabled");
            }
        }
        else {
            PlatformDisableRawInput(Input->Window);
        }
    }
}

void SetCursorVisibility(FInput *Input, ECursorVisibility Visibility) {
    if (Visibility != Input->CursorVisibility) {
        Input->CursorVisibility = Visibility;
        if (Input->CursorVisibility != ECursorVisibility::DISABLED && Input->UsingRawInput) {
            SK_LOG_ERROR("Cannot change cursor state from disabled while raw input is enabled");
            return;
        }
        PlatformSetCursorVisibility(Input->Window, Visibility);
    }
}
