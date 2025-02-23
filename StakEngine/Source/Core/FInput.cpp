#include "FInput.h"

#include "FLog.h"
#undef DELETE

Input::Input() {
    ASSERT(!instance);
    instance = this;
    
    keys[Key_Code::UNKNOWN] = Input_State::UP;
    keys[Key_Code::SPACE] = Input_State::UP;
    keys[Key_Code::APOSTROPHE] = Input_State::UP;
    keys[Key_Code::COMMA] = Input_State::UP;
    keys[Key_Code::MINUS] = Input_State::UP;
    keys[Key_Code::PERIOD] = Input_State::UP;
    keys[Key_Code::SLASH] = Input_State::UP;
    keys[Key_Code::ZERO] = Input_State::UP;
    keys[Key_Code::ONE] = Input_State::UP;
    keys[Key_Code::TWO] = Input_State::UP;
    keys[Key_Code::THREE] = Input_State::UP;
    keys[Key_Code::FOUR] = Input_State::UP;
    keys[Key_Code::FIVE] = Input_State::UP;
    keys[Key_Code::SIX] = Input_State::UP;
    keys[Key_Code::SEVEN] = Input_State::UP;
    keys[Key_Code::EIGHT] = Input_State::UP;
    keys[Key_Code::NINE] = Input_State::UP;
    keys[Key_Code::SEMICOLON] = Input_State::UP;
    keys[Key_Code::EQUAL] = Input_State::UP;
    keys[Key_Code::A] = Input_State::UP;
    keys[Key_Code::B] = Input_State::UP;
    keys[Key_Code::C] = Input_State::UP;
    keys[Key_Code::D] = Input_State::UP;
    keys[Key_Code::E] = Input_State::UP;
    keys[Key_Code::F] = Input_State::UP;
    keys[Key_Code::G] = Input_State::UP;
    keys[Key_Code::H] = Input_State::UP;
    keys[Key_Code::I] = Input_State::UP;
    keys[Key_Code::J] = Input_State::UP;
    keys[Key_Code::K] = Input_State::UP;
    keys[Key_Code::L] = Input_State::UP;
    keys[Key_Code::M] = Input_State::UP;
    keys[Key_Code::N] = Input_State::UP;
    keys[Key_Code::O] = Input_State::UP;
    keys[Key_Code::P] = Input_State::UP;
    keys[Key_Code::Q] = Input_State::UP;
    keys[Key_Code::R] = Input_State::UP;
    keys[Key_Code::S] = Input_State::UP;
    keys[Key_Code::T] = Input_State::UP;
    keys[Key_Code::U] = Input_State::UP;
    keys[Key_Code::V] = Input_State::UP;
    keys[Key_Code::W] = Input_State::UP;
    keys[Key_Code::X] = Input_State::UP;
    keys[Key_Code::Y] = Input_State::UP;
    keys[Key_Code::Z] = Input_State::UP;
    keys[Key_Code::LEFT_BRACKET] = Input_State::UP;
    keys[Key_Code::BACKSLASH] = Input_State::UP;
    keys[Key_Code::RIGHT_BRACKET] = Input_State::UP;
    keys[Key_Code::GRAVE_ACCENT] = Input_State::UP;
    keys[Key_Code::WORLD_1] = Input_State::UP;
    keys[Key_Code::WORLD_2] = Input_State::UP;
    keys[Key_Code::ESCAPE] = Input_State::UP;
    keys[Key_Code::ENTER] = Input_State::UP;
    keys[Key_Code::TAB] = Input_State::UP;
    keys[Key_Code::BACKSPACE] = Input_State::UP;
    keys[Key_Code::INSERT] = Input_State::UP;
    keys[Key_Code::DELETE] = Input_State::UP;
    keys[Key_Code::RIGHT] = Input_State::UP;
    keys[Key_Code::LEFT] = Input_State::UP;
    keys[Key_Code::DOWN] = Input_State::UP;
    keys[Key_Code::UP] = Input_State::UP;
    keys[Key_Code::PAGE_UP] = Input_State::UP;
    keys[Key_Code::PAGE_DOWN] = Input_State::UP;
    keys[Key_Code::HOME] = Input_State::UP;
    keys[Key_Code::END] = Input_State::UP;
    keys[Key_Code::CAPS_LOCK] = Input_State::UP;
    keys[Key_Code::SCROLL_LOCK] = Input_State::UP;
    keys[Key_Code::NUM_LOCK] = Input_State::UP;
    keys[Key_Code::PRINT_SCREEN] = Input_State::UP;
    keys[Key_Code::PAUSE] = Input_State::UP;
    keys[Key_Code::F1] = Input_State::UP;
    keys[Key_Code::F2] = Input_State::UP;
    keys[Key_Code::F3] = Input_State::UP;
    keys[Key_Code::F4] = Input_State::UP;
    keys[Key_Code::F5] = Input_State::UP;
    keys[Key_Code::F6] = Input_State::UP;
    keys[Key_Code::F7] = Input_State::UP;
    keys[Key_Code::F8] = Input_State::UP;
    keys[Key_Code::F9] = Input_State::UP;
    keys[Key_Code::F10] = Input_State::UP;
    keys[Key_Code::F11] = Input_State::UP;
    keys[Key_Code::F12] = Input_State::UP;
    keys[Key_Code::F13] = Input_State::UP;
    keys[Key_Code::F14] = Input_State::UP;
    keys[Key_Code::F15] = Input_State::UP;
    keys[Key_Code::F16] = Input_State::UP;
    keys[Key_Code::F17] = Input_State::UP;
    keys[Key_Code::F18] = Input_State::UP;
    keys[Key_Code::F19] = Input_State::UP;
    keys[Key_Code::F20] = Input_State::UP;
    keys[Key_Code::F21] = Input_State::UP;
    keys[Key_Code::F22] = Input_State::UP;
    keys[Key_Code::F23] = Input_State::UP;
    keys[Key_Code::F24] = Input_State::UP;
    keys[Key_Code::F25] = Input_State::UP;
    keys[Key_Code::KP_0] = Input_State::UP;
    keys[Key_Code::KP_1] = Input_State::UP;
    keys[Key_Code::KP_2] = Input_State::UP;
    keys[Key_Code::KP_3] = Input_State::UP;
    keys[Key_Code::KP_4] = Input_State::UP;
    keys[Key_Code::KP_5] = Input_State::UP;
    keys[Key_Code::KP_6] = Input_State::UP;
    keys[Key_Code::KP_7] = Input_State::UP;
    keys[Key_Code::KP_8] = Input_State::UP;
    keys[Key_Code::KP_9] = Input_State::UP;
    keys[Key_Code::KP_DECIMAL] = Input_State::UP;
    keys[Key_Code::KP_DIVIDE] = Input_State::UP;
    keys[Key_Code::KP_MULTIPLY] = Input_State::UP;
    keys[Key_Code::KP_SUBTRACT] = Input_State::UP;
    keys[Key_Code::KP_ADD] = Input_State::UP;
    keys[Key_Code::KP_ENTER] = Input_State::UP;
    keys[Key_Code::KP_EQUAL] = Input_State::UP;
    keys[Key_Code::LEFT_SHIFT] = Input_State::UP;
    keys[Key_Code::LEFT_CONTROL] = Input_State::UP;
    keys[Key_Code::LEFT_ALT] = Input_State::UP;
    keys[Key_Code::LEFT_SUPER] = Input_State::UP;
    keys[Key_Code::RIGHT_SHIFT] = Input_State::UP;
    keys[Key_Code::RIGHT_CONTROL] = Input_State::UP;
    keys[Key_Code::RIGHT_ALT] = Input_State::UP;
    keys[Key_Code::RIGHT_SUPER] = Input_State::UP;
    keys[Key_Code::MENU] = Input_State::UP;
    keys[Key_Code::LAST] = Input_State::UP;

    mouse_buttons[Mouse_Code::ONE] = Input_State::UP;
    mouse_buttons[Mouse_Code::TWO] = Input_State::UP;
    mouse_buttons[Mouse_Code::THREE] = Input_State::UP;
    mouse_buttons[Mouse_Code::FOUR] = Input_State::UP;
    mouse_buttons[Mouse_Code::FIVE] = Input_State::UP;
    mouse_buttons[Mouse_Code::SIX] = Input_State::UP;
    mouse_buttons[Mouse_Code::SEVEN] = Input_State::UP;
    mouse_buttons[Mouse_Code::EIGHT] = Input_State::UP;
    mouse_buttons[Mouse_Code::LAST] = Input_State::UP;
    mouse_buttons[Mouse_Code::LEFT] = Input_State::UP;
    mouse_buttons[Mouse_Code::RIGHT] = Input_State::UP;
    mouse_buttons[Mouse_Code::MIDDLE] = Input_State::UP;

    raw_input = false;
    cursor_visibility = Cursor_Visibility::NORMAL;
}

Input::~Input() {
    instance = NULL;
}

void Input::add_key_state_callback(Input_State state, Key_Code key, std::function<void()> func) {
    switch (state) {
    case Input_State::DOWN:
        key_press_callbacks[key].push_back(func);
        break;
    case Input_State::UP:
        key_release_callbacks[key].push_back(func);
        break;
    }
}

void Input::add_generic_key_state_callback(Input_State state, std::function<void(Key_Code)> func) {
    switch (state) {
    case Input_State::DOWN:
        generic_key_press_callbacks.push_back(func);
        break;
    case Input_State::UP:
        generic_key_release_callbacks.push_back(func);
        break;
    }
}

void Input::add_generic_key_callback(std::function<void(Input_State, Key_Code)> func) {
    generic_key_callbacks.push_back(func);
}

void Input::add_mouse_button_state_callback(Input_State state, Mouse_Code button, std::function<void()> func) {
    switch (state) {
    case Input_State::DOWN:
        mouse_button_pressed_callbacks[button].push_back(func);
        break;
    case Input_State::UP:
        mouse_button_released_callbacks[button].push_back(func);
        break;
    }
}

void Input::add_generic_mouse_button_state_callback(Input_State state, std::function<void(Mouse_Code)> func) {
    switch (state) {
    case Input_State::DOWN:
        generic_mouse_button_press_callbacks.push_back(func);
        break;
    case Input_State::UP:
        generic_mouse_button_release_callbacks.push_back(func);
        break;
    }
}

void Input::add_generic_mouse_button_callback(std::function<void(Input_State, Mouse_Code)> func) {
    generic_mouse_button_callbacks.push_back(func);
}

void Input::add_mouse_move_callback(std::function<void(f32, f32)> func) {
    mouse_move_callbacks.push_back(func);
}

void Input::set_key(Key_Code key, Input_State state) {
    keys[key] = state;

    switch (state) {
    case Input_State::DOWN:
        for (const auto &func : key_press_callbacks[key]) {
            func();
        }
        for (const auto &func : generic_key_press_callbacks) {
            func(key);
        }
        break;
    case Input_State::UP:
        for (const auto &func : key_release_callbacks[key]) {
            func();
        }
        for (const auto &func : generic_key_release_callbacks) {
            func(key);
        }
        break;
    }

    for (const auto &func : generic_key_callbacks) {
        func(state, key);
    }
}

void Input::set_mouse_button(Mouse_Code button, Input_State state) {
    mouse_buttons[button] = state;

    switch (state) {
    case Input_State::DOWN:
        for (const auto &func : mouse_button_pressed_callbacks[button]) {
            func();
        }
        for (const auto &func : generic_mouse_button_press_callbacks) {
            func(button);
        }
        break;
    case Input_State::UP:
        for (const auto &func : mouse_button_released_callbacks[button]) {
            func();
        }
        for (const auto &func : generic_mouse_button_release_callbacks) {
            func(button);
        }
        break;
    }
    
    for (const auto &func : generic_mouse_button_callbacks) {
        func(state, button);
    }
}

void Input::set_mouse_pos(f32 x, f32 y) {
    mouse_x = x;
    mouse_y = y;

    for (const auto &func : mouse_move_callbacks) {
        func(x, y);
    }
}

#ifdef SK_GLFW
#include "GLFWInput.h"
#define PlatformEnableRawInput glfw_enable_raw_input
#define PlatformDisableRawInput glfw_enable_raw_input
#define PlatformSetCursorVisibility glfw_set_cursor_visibility
#endif

void Input::set_raw_input(bool value) {
    if (raw_input != value) {
        raw_input = value;
        if (value) {
            if (cursor_visibility == Cursor_Visibility::DISABLED) {
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

void Input::set_cursor_visibility(Cursor_Visibility visibility) {
    if (visibility != cursor_visibility) {
        cursor_visibility = visibility;
        if (cursor_visibility != Cursor_Visibility::DISABLED && raw_input) {
            SK_LOG_ERROR("Cannot change cursor state from disabled while raw input is enabled");
            return;
        }
        PlatformSetCursorVisibility(visibility);
    }
}
