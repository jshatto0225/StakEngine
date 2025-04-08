#include "Input.h"

#include "Log.h"
#include "Window.h"
#include "Platform.h"
#undef Delete


bool initialize_input(Input *input, Window *window) {
    input->window = window;

    input->keyboard[KeyCode::Unknown] = InputState::Up;
    input->keyboard[KeyCode::Space] = InputState::Up;
    input->keyboard[KeyCode::Apostrophe] = InputState::Up;
    input->keyboard[KeyCode::Comma] = InputState::Up;
    input->keyboard[KeyCode::Minus] = InputState::Up;
    input->keyboard[KeyCode::Period] = InputState::Up;
    input->keyboard[KeyCode::Slash] = InputState::Up;
    input->keyboard[KeyCode::Zero] = InputState::Up;
    input->keyboard[KeyCode::One] = InputState::Up;
    input->keyboard[KeyCode::Two] = InputState::Up;
    input->keyboard[KeyCode::Three] = InputState::Up;
    input->keyboard[KeyCode::Four] = InputState::Up;
    input->keyboard[KeyCode::Five] = InputState::Up;
    input->keyboard[KeyCode::Six] = InputState::Up;
    input->keyboard[KeyCode::Seven] = InputState::Up;
    input->keyboard[KeyCode::Eight] = InputState::Up;
    input->keyboard[KeyCode::Nine] = InputState::Up;
    input->keyboard[KeyCode::Semicolon] = InputState::Up;
    input->keyboard[KeyCode::Equal] = InputState::Up;
    input->keyboard[KeyCode::A] = InputState::Up;
    input->keyboard[KeyCode::B] = InputState::Up;
    input->keyboard[KeyCode::C] = InputState::Up;
    input->keyboard[KeyCode::D] = InputState::Up;
    input->keyboard[KeyCode::E] = InputState::Up;
    input->keyboard[KeyCode::F] = InputState::Up;
    input->keyboard[KeyCode::G] = InputState::Up;
    input->keyboard[KeyCode::H] = InputState::Up;
    input->keyboard[KeyCode::I] = InputState::Up;
    input->keyboard[KeyCode::J] = InputState::Up;
    input->keyboard[KeyCode::K] = InputState::Up;
    input->keyboard[KeyCode::L] = InputState::Up;
    input->keyboard[KeyCode::M] = InputState::Up;
    input->keyboard[KeyCode::N] = InputState::Up;
    input->keyboard[KeyCode::O] = InputState::Up;
    input->keyboard[KeyCode::P] = InputState::Up;
    input->keyboard[KeyCode::Q] = InputState::Up;
    input->keyboard[KeyCode::R] = InputState::Up;
    input->keyboard[KeyCode::S] = InputState::Up;
    input->keyboard[KeyCode::T] = InputState::Up;
    input->keyboard[KeyCode::U] = InputState::Up;
    input->keyboard[KeyCode::V] = InputState::Up;
    input->keyboard[KeyCode::W] = InputState::Up;
    input->keyboard[KeyCode::x] = InputState::Up;
    input->keyboard[KeyCode::y] = InputState::Up;
    input->keyboard[KeyCode::Z] = InputState::Up;
    input->keyboard[KeyCode::LeftBracket] = InputState::Up;
    input->keyboard[KeyCode::Backslash] = InputState::Up;
    input->keyboard[KeyCode::RightBracket] = InputState::Up;
    input->keyboard[KeyCode::Grave] = InputState::Up;
    input->keyboard[KeyCode::World1] = InputState::Up;
    input->keyboard[KeyCode::World2] = InputState::Up;
    input->keyboard[KeyCode::Escape] = InputState::Up;
    input->keyboard[KeyCode::Enter] = InputState::Up;
    input->keyboard[KeyCode::Tab] = InputState::Up;
    input->keyboard[KeyCode::Backspace] = InputState::Up;
    input->keyboard[KeyCode::Insert] = InputState::Up;
    input->keyboard[KeyCode::Delete] = InputState::Up;
    input->keyboard[KeyCode::Right] = InputState::Up;
    input->keyboard[KeyCode::Left] = InputState::Up;
    input->keyboard[KeyCode::Down] = InputState::Up;
    input->keyboard[KeyCode::Up] = InputState::Up;
    input->keyboard[KeyCode::PageUp] = InputState::Up;
    input->keyboard[KeyCode::PageDown] = InputState::Up;
    input->keyboard[KeyCode::Home] = InputState::Up;
    input->keyboard[KeyCode::End] = InputState::Up;
    input->keyboard[KeyCode::CapsLock] = InputState::Up;
    input->keyboard[KeyCode::ScrollLock] = InputState::Up;
    input->keyboard[KeyCode::NumLock] = InputState::Up;
    input->keyboard[KeyCode::PrintScreen] = InputState::Up;
    input->keyboard[KeyCode::Pause] = InputState::Up;
    input->keyboard[KeyCode::F1] = InputState::Up;
    input->keyboard[KeyCode::F2] = InputState::Up;
    input->keyboard[KeyCode::F3] = InputState::Up;
    input->keyboard[KeyCode::F4] = InputState::Up;
    input->keyboard[KeyCode::F5] = InputState::Up;
    input->keyboard[KeyCode::F6] = InputState::Up;
    input->keyboard[KeyCode::F7] = InputState::Up;
    input->keyboard[KeyCode::F8] = InputState::Up;
    input->keyboard[KeyCode::F9] = InputState::Up;
    input->keyboard[KeyCode::F10] = InputState::Up;
    input->keyboard[KeyCode::F11] = InputState::Up;
    input->keyboard[KeyCode::F12] = InputState::Up;
    input->keyboard[KeyCode::F13] = InputState::Up;
    input->keyboard[KeyCode::F14] = InputState::Up;
    input->keyboard[KeyCode::F15] = InputState::Up;
    input->keyboard[KeyCode::F16] = InputState::Up;
    input->keyboard[KeyCode::F17] = InputState::Up;
    input->keyboard[KeyCode::F18] = InputState::Up;
    input->keyboard[KeyCode::F19] = InputState::Up;
    input->keyboard[KeyCode::F20] = InputState::Up;
    input->keyboard[KeyCode::F21] = InputState::Up;
    input->keyboard[KeyCode::F22] = InputState::Up;
    input->keyboard[KeyCode::F23] = InputState::Up;
    input->keyboard[KeyCode::F24] = InputState::Up;
    input->keyboard[KeyCode::F25] = InputState::Up;
    input->keyboard[KeyCode::KP_0] = InputState::Up;
    input->keyboard[KeyCode::KP_1] = InputState::Up;
    input->keyboard[KeyCode::KP_2] = InputState::Up;
    input->keyboard[KeyCode::KP_3] = InputState::Up;
    input->keyboard[KeyCode::KP_4] = InputState::Up;
    input->keyboard[KeyCode::KP_5] = InputState::Up;
    input->keyboard[KeyCode::KP_6] = InputState::Up;
    input->keyboard[KeyCode::KP_7] = InputState::Up;
    input->keyboard[KeyCode::KP_8] = InputState::Up;
    input->keyboard[KeyCode::KP_9] = InputState::Up;
    input->keyboard[KeyCode::KPDecimal] = InputState::Up;
    input->keyboard[KeyCode::KPDivide] = InputState::Up;
    input->keyboard[KeyCode::KPMultiply] = InputState::Up;
    input->keyboard[KeyCode::KPSubtract] = InputState::Up;
    input->keyboard[KeyCode::KPAdd] = InputState::Up;
    input->keyboard[KeyCode::KPEnter] = InputState::Up;
    input->keyboard[KeyCode::KPEqual] = InputState::Up;
    input->keyboard[KeyCode::LeftShift] = InputState::Up;
    input->keyboard[KeyCode::LeftControl] = InputState::Up;
    input->keyboard[KeyCode::LeftAlt] = InputState::Up;
    input->keyboard[KeyCode::LeftSuper] = InputState::Up;
    input->keyboard[KeyCode::RightShift] = InputState::Up;
    input->keyboard[KeyCode::RightControl] = InputState::Up;
    input->keyboard[KeyCode::RightAlt] = InputState::Up;
    input->keyboard[KeyCode::RightSuper] = InputState::Up;
    input->keyboard[KeyCode::Menu] = InputState::Up;
    input->keyboard[KeyCode::Last] = InputState::Up;

    input->mouse.buttons[MouseCode::One] = InputState::Up;
    input->mouse.buttons[MouseCode::Two] = InputState::Up;
    input->mouse.buttons[MouseCode::Three] = InputState::Up;
    input->mouse.buttons[MouseCode::Four] = InputState::Up;
    input->mouse.buttons[MouseCode::Five] = InputState::Up;
    input->mouse.buttons[MouseCode::Six] = InputState::Up;
    input->mouse.buttons[MouseCode::Seven] = InputState::Up;
    input->mouse.buttons[MouseCode::Eight] = InputState::Up;
    input->mouse.buttons[MouseCode::Last] = InputState::Up;
    input->mouse.buttons[MouseCode::Left] = InputState::Up;
    input->mouse.buttons[MouseCode::Right] = InputState::Up;
    input->mouse.buttons[MouseCode::Middle] = InputState::Up;

    input->using_raw_input = false;
    input->cursor_visibility = CursorVisibility::Normal;

    return true;
}

InputCallbackInfo add_key_press_callback(Input *input, KeyCode key, std::function<void()> func) {
    input->key_press_callbacks[key][input->key_press_id] = func;

    InputCallbackInfo info = {};

    info.type = InputCallbackType::Key;
    info.key = key;
    info.action = InputState::Down;
    info.id = input->key_press_id;

    input->key_press_id++;

    return info;
}

InputCallbackInfo add_key_release_callback(Input *input, KeyCode key, std::function<void()> func) {
    input->key_release_callbacks[key][input->key_release_id] = func;

    InputCallbackInfo info = {};

    info.type = InputCallbackType::Key;
    info.key = key;
    info.action = InputState::Up;
    info.id = input->key_release_id;

    input->key_release_id++;

    return info;
}

InputCallbackInfo add_any_key_press_callback(Input *input, std::function<void(KeyCode)> func) {
    input->any_key_press_callbacks[input->any_key_press_id] = func;

    InputCallbackInfo info = {};

    info.type = InputCallbackType::Key;
    info.action = InputState::Down;
    info.is_any_code = true;
    info.id = input->any_key_press_id;

    input->any_key_press_id++;

    return info;
}

InputCallbackInfo add_any_key_release_callback(Input *input, std::function<void(KeyCode)> func) {
    input->any_key_release_callbacks[input->any_key_release_id] = func;

    InputCallbackInfo info = {};

    info.type = InputCallbackType::Key;
    info.action = InputState::Up;
    info.is_any_code = true;
    info.id = input->any_key_release_id;

    input->any_key_release_id++;

    return info;
}

InputCallbackInfo add_key_any_action_callback(Input *input, KeyCode key, std::function<void(InputState)> func) {
    input->key_any_action_callbacks[key][input->key_any_action_id] = func;

    InputCallbackInfo info = {};

    info.type = InputCallbackType::Key;
    info.is_any_code = true;
    info.key = key;
    info.id = input->key_any_action_id;

    input->key_any_action_id++;

    return info;
}

InputCallbackInfo add_any_key_any_action_callback(Input *input, std::function<void(InputState, KeyCode)> func) {
    input->any_key_any_action_callbacks[input->any_key_any_action_id] = func;

    InputCallbackInfo info = {};

    info.type = InputCallbackType::Key;
    info.is_any_code = true;
    info.is_any_action = true;
    info.id = input->any_key_any_action_id;

    input->any_key_any_action_id++;

    return info;
}

InputCallbackInfo add_mouse_button_press_callback(Input *input, MouseCode button, std::function<void()> func) {
    input->mouse_button_press_callbacks[button][input->mouse_button_press_id] = func;

    InputCallbackInfo info = {};

    info.type = InputCallbackType::MouseButton;
    info.button = button;
    info.action = InputState::Down;
    info.id = input->mouse_button_press_id;

    input->mouse_button_press_id++;

    return info;
}

InputCallbackInfo add_mouse_button_release_callback(Input *input, MouseCode button, std::function<void()> func) {
    input->mouse_button_release_callbacks[button][input->mouse_button_release_id] = func;

    InputCallbackInfo info = {};

    info.type = InputCallbackType::MouseButton;
    info.button = button;
    info.action = InputState::Up;
    info.id = input->mouse_button_release_id;

    input->mouse_button_release_id++;

    return info;
}

InputCallbackInfo add_any_mouse_button_press_callback(Input *input, std::function<void(MouseCode)> func) {
    input->any_mouse_button_press_callbacks[input->any_mouse_button_press_id] = func;

    InputCallbackInfo info = {};

    info.type = InputCallbackType::MouseButton;
    info.action = InputState::Down;
    info.is_any_code = true;
    info.id = input->any_mouse_button_press_id;

    input->any_mouse_button_press_id++;

    return info;
}

InputCallbackInfo add_any_mouse_button_release_callback(Input *input, std::function<void(MouseCode)> func) {
    input->any_mouse_button_release_callbacks[input->any_mouse_button_release_id] = func;

    InputCallbackInfo info = {};

    info.type = InputCallbackType::MouseButton;
    info.action = InputState::Up;
    info.is_any_code = true;
    info.id = input->any_mouse_button_release_id;

    input->any_mouse_button_release_id++;

    return info;
}

InputCallbackInfo add_mouse_button_any_action_callback(Input *input, MouseCode button, std::function<void(InputState)> func) {
    input->mouse_button_any_action_callbakcs[button][input->mouse_button_any_action_id] = func;

    InputCallbackInfo info = {};

    info.type = InputCallbackType::MouseButton;
    info.is_any_action = true;
    info.button = button;
    info.id = input->mouse_button_any_action_id;

    input->mouse_button_any_action_id++;

    return info;
}

InputCallbackInfo add_any_mouse_button_any_action_callback(Input *input, std::function<void(InputState, MouseCode)> func) {
    input->any_mouse_button_any_action_callbacks[input->any_mouse_button_any_action_id] = func;

    InputCallbackInfo info = {};

    info.type = InputCallbackType::MouseButton;
    info.is_any_action = true;
    info.is_any_code = true;
    info.id = input->any_mouse_button_any_action_id;

    input->any_mouse_button_any_action_id++;

    return info;
}

InputCallbackInfo add_mouse_move_callback(Input *input, std::function<void(f32, f32)> func) {
    input->mouse_move_callbacks[input->mouse_move_id] = func;

    InputCallbackInfo info = {};

    info.type = InputCallbackType::MouseMove;
    info.id = input->mouse_move_id;

    input->mouse_move_id++;

    return info;
}

void SetKey(Input *input, KeyCode key, InputState state) {
    input->keyboard[key] = state;

    switch (state) {
    case InputState::Down:
        for (auto &it : input->key_press_callbacks[key]) {
            it.second();
        }
        for (auto &It : input->any_key_press_callbacks) {
            It.second(key);
        }
        break;
    case InputState::Up:
        for (auto &it : input->key_release_callbacks[key]) {
            it.second();
        }
        for (auto &it : input->any_key_release_callbacks) {
            it.second(key);
        }
        break;
    }

    for (auto &it : input->key_any_action_callbacks[key]) {
        it.second(state);
    }
    for (auto &It : input->any_key_any_action_callbacks) {
        It.second(state, key);
    }
}

void SetMouseButton(Input *input, MouseCode button, InputState state) {
    input->mouse.buttons[button] = state;

    switch (state) {
    case InputState::Down:
        for (auto &it : input->mouse_button_press_callbacks[button]) {
            it.second();
        }
        for (auto &it : input->any_mouse_button_press_callbacks) {
            it.second(button);
        }
        break;
    case InputState::Up:
        for (auto &it : input->mouse_button_release_callbacks[button]) {
            it.second();
        }
        for (auto &it : input->any_mouse_button_release_callbacks) {
            it.second(button);
        }
        break;
    }

    for (auto &it : input->mouse_button_any_action_callbakcs[button]) {
        it.second(state);
    }
    for (const auto &it : input->any_mouse_button_any_action_callbacks) {
        it.second(state, button);
    }
}

void SetMousePos(Input *input, f32 x, f32 y) {
    input->mouse.x = x;
    input->mouse.y = y;

    for (auto &it : input->mouse_move_callbacks) {
        it.second(x, y);
    }
}

void RemoveCallback(Input *input, const InputCallbackInfo &info) {
    switch (info.type) {
    case InputCallbackType::Key:
        if (info.is_any_action && info.is_any_code) {
            input->any_key_any_action_callbacks.erase(info.id);
        } else if (info.is_any_action) {
            input->key_any_action_callbacks[info.key].erase(info.id);
        } else if (info.is_any_code) {
            switch (info.action) {
            case InputState::Up:
                input->any_key_release_callbacks.erase(info.id);
                break;
            case InputState::Down:
                input->any_key_press_callbacks.erase(info.id);
                break;
            }
        } else {
            switch (info.action) {
            case InputState::Up:
                input->key_release_callbacks[info.key].erase(info.id);
                break;
            case InputState::Down:
                input->key_press_callbacks[info.key].erase(info.id);
                break;
            }
        }
        break;
    case InputCallbackType::MouseButton:
        if (info.is_any_action && info.is_any_code) {
            input->any_mouse_button_any_action_callbacks.erase(info.id);
        } else if (info.is_any_action) {
            input->mouse_button_any_action_callbakcs[info.button].erase(info.id);
        } else if (info.is_any_code) {
            switch (info.action) {
            case InputState::Up:
                input->any_mouse_button_release_callbacks.erase(info.id);
                break;
            case InputState::Down:
                input->any_mouse_button_press_callbacks.erase(info.id);
                break;
            }
        } else {
            switch (info.action) {
            case InputState::Up:
                input->mouse_button_release_callbacks[info.button].erase(info.id);
                break;
            case InputState::Down:
                input->mouse_button_press_callbacks[info.button].erase(info.id);
                break;
            }
        }
        break;
    case InputCallbackType::MouseMove:
        input->mouse_move_callbacks.erase(info.id);
        break;
    }
}

void set_raw_input(Input *input, bool value) {
    if (input->using_raw_input != value) {
        input->using_raw_input = value;
        if (value) {
            if (input->cursor_visibility == CursorVisibility::Disabled) {
                platform_enable_raw_input(input->window);
            }
            else {
                SK_LOG_ERROR("Cannot enable raw input while cursor is not disabled");
            }
        }
        else {
            platform_disable_raw_input(input->window);
        }
    }
}

void set_cursor_visibility(Input *input, CursorVisibility visibility) {
    if (visibility != input->cursor_visibility) {
        input->cursor_visibility = visibility;
        if (input->cursor_visibility != CursorVisibility::Disabled && input->using_raw_input) {
            SK_LOG_ERROR("Cannot change cursor state from disabled while raw input is enabled");
            return;
        }
        platform_set_cursor_visibility(input->window, visibility);
    }
}
