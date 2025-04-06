#include "Input.h"

#include "Log.h"
#include "Window.h"
#include "Platform.h"
#undef DELETE


bool initialize_input(Input *input, Window *window) {
    input->window = window;

    input->keyboard[Key_Code::UNKNOWN] = Input_State::UP;
    input->keyboard[Key_Code::SPACE] = Input_State::UP;
    input->keyboard[Key_Code::APOSTROPHE] = Input_State::UP;
    input->keyboard[Key_Code::COMMA] = Input_State::UP;
    input->keyboard[Key_Code::MINUS] = Input_State::UP;
    input->keyboard[Key_Code::PERIOD] = Input_State::UP;
    input->keyboard[Key_Code::SLASH] = Input_State::UP;
    input->keyboard[Key_Code::ZERO] = Input_State::UP;
    input->keyboard[Key_Code::ONE] = Input_State::UP;
    input->keyboard[Key_Code::TWO] = Input_State::UP;
    input->keyboard[Key_Code::THREE] = Input_State::UP;
    input->keyboard[Key_Code::FOUR] = Input_State::UP;
    input->keyboard[Key_Code::FIVE] = Input_State::UP;
    input->keyboard[Key_Code::SIX] = Input_State::UP;
    input->keyboard[Key_Code::SEVEN] = Input_State::UP;
    input->keyboard[Key_Code::EIGHT] = Input_State::UP;
    input->keyboard[Key_Code::NINE] = Input_State::UP;
    input->keyboard[Key_Code::SEMICOLON] = Input_State::UP;
    input->keyboard[Key_Code::EQUAL] = Input_State::UP;
    input->keyboard[Key_Code::A] = Input_State::UP;
    input->keyboard[Key_Code::B] = Input_State::UP;
    input->keyboard[Key_Code::C] = Input_State::UP;
    input->keyboard[Key_Code::D] = Input_State::UP;
    input->keyboard[Key_Code::E] = Input_State::UP;
    input->keyboard[Key_Code::F] = Input_State::UP;
    input->keyboard[Key_Code::G] = Input_State::UP;
    input->keyboard[Key_Code::H] = Input_State::UP;
    input->keyboard[Key_Code::I] = Input_State::UP;
    input->keyboard[Key_Code::J] = Input_State::UP;
    input->keyboard[Key_Code::K] = Input_State::UP;
    input->keyboard[Key_Code::L] = Input_State::UP;
    input->keyboard[Key_Code::M] = Input_State::UP;
    input->keyboard[Key_Code::N] = Input_State::UP;
    input->keyboard[Key_Code::O] = Input_State::UP;
    input->keyboard[Key_Code::P] = Input_State::UP;
    input->keyboard[Key_Code::Q] = Input_State::UP;
    input->keyboard[Key_Code::R] = Input_State::UP;
    input->keyboard[Key_Code::S] = Input_State::UP;
    input->keyboard[Key_Code::T] = Input_State::UP;
    input->keyboard[Key_Code::U] = Input_State::UP;
    input->keyboard[Key_Code::V] = Input_State::UP;
    input->keyboard[Key_Code::W] = Input_State::UP;
    input->keyboard[Key_Code::x] = Input_State::UP;
    input->keyboard[Key_Code::y] = Input_State::UP;
    input->keyboard[Key_Code::Z] = Input_State::UP;
    input->keyboard[Key_Code::LEFT_BRACKET] = Input_State::UP;
    input->keyboard[Key_Code::BACKSLASH] = Input_State::UP;
    input->keyboard[Key_Code::RIGHT_BRACKET] = Input_State::UP;
    input->keyboard[Key_Code::GRAVE_ACCENT] = Input_State::UP;
    input->keyboard[Key_Code::WORLD_1] = Input_State::UP;
    input->keyboard[Key_Code::WORLD_2] = Input_State::UP;
    input->keyboard[Key_Code::ESCAPE] = Input_State::UP;
    input->keyboard[Key_Code::ENTER] = Input_State::UP;
    input->keyboard[Key_Code::TAB] = Input_State::UP;
    input->keyboard[Key_Code::BACKSPACE] = Input_State::UP;
    input->keyboard[Key_Code::INSERT] = Input_State::UP;
    input->keyboard[Key_Code::DELETE] = Input_State::UP;
    input->keyboard[Key_Code::RIGHT] = Input_State::UP;
    input->keyboard[Key_Code::LEFT] = Input_State::UP;
    input->keyboard[Key_Code::DOWN] = Input_State::UP;
    input->keyboard[Key_Code::UP] = Input_State::UP;
    input->keyboard[Key_Code::PAGE_UP] = Input_State::UP;
    input->keyboard[Key_Code::PAGE_DOWN] = Input_State::UP;
    input->keyboard[Key_Code::HOME] = Input_State::UP;
    input->keyboard[Key_Code::END] = Input_State::UP;
    input->keyboard[Key_Code::CAPS_LOCK] = Input_State::UP;
    input->keyboard[Key_Code::SCROLL_LOCK] = Input_State::UP;
    input->keyboard[Key_Code::NUM_LOCK] = Input_State::UP;
    input->keyboard[Key_Code::PRINT_SCREEN] = Input_State::UP;
    input->keyboard[Key_Code::PAUSE] = Input_State::UP;
    input->keyboard[Key_Code::F1] = Input_State::UP;
    input->keyboard[Key_Code::F2] = Input_State::UP;
    input->keyboard[Key_Code::F3] = Input_State::UP;
    input->keyboard[Key_Code::F4] = Input_State::UP;
    input->keyboard[Key_Code::F5] = Input_State::UP;
    input->keyboard[Key_Code::F6] = Input_State::UP;
    input->keyboard[Key_Code::F7] = Input_State::UP;
    input->keyboard[Key_Code::F8] = Input_State::UP;
    input->keyboard[Key_Code::F9] = Input_State::UP;
    input->keyboard[Key_Code::F10] = Input_State::UP;
    input->keyboard[Key_Code::F11] = Input_State::UP;
    input->keyboard[Key_Code::F12] = Input_State::UP;
    input->keyboard[Key_Code::F13] = Input_State::UP;
    input->keyboard[Key_Code::F14] = Input_State::UP;
    input->keyboard[Key_Code::F15] = Input_State::UP;
    input->keyboard[Key_Code::F16] = Input_State::UP;
    input->keyboard[Key_Code::F17] = Input_State::UP;
    input->keyboard[Key_Code::F18] = Input_State::UP;
    input->keyboard[Key_Code::F19] = Input_State::UP;
    input->keyboard[Key_Code::F20] = Input_State::UP;
    input->keyboard[Key_Code::F21] = Input_State::UP;
    input->keyboard[Key_Code::F22] = Input_State::UP;
    input->keyboard[Key_Code::F23] = Input_State::UP;
    input->keyboard[Key_Code::F24] = Input_State::UP;
    input->keyboard[Key_Code::F25] = Input_State::UP;
    input->keyboard[Key_Code::KP_0] = Input_State::UP;
    input->keyboard[Key_Code::KP_1] = Input_State::UP;
    input->keyboard[Key_Code::KP_2] = Input_State::UP;
    input->keyboard[Key_Code::KP_3] = Input_State::UP;
    input->keyboard[Key_Code::KP_4] = Input_State::UP;
    input->keyboard[Key_Code::KP_5] = Input_State::UP;
    input->keyboard[Key_Code::KP_6] = Input_State::UP;
    input->keyboard[Key_Code::KP_7] = Input_State::UP;
    input->keyboard[Key_Code::KP_8] = Input_State::UP;
    input->keyboard[Key_Code::KP_9] = Input_State::UP;
    input->keyboard[Key_Code::KP_DECIMAL] = Input_State::UP;
    input->keyboard[Key_Code::KP_DIVIDE] = Input_State::UP;
    input->keyboard[Key_Code::KP_MULTIPLY] = Input_State::UP;
    input->keyboard[Key_Code::KP_SUBTRACT] = Input_State::UP;
    input->keyboard[Key_Code::KP_ADD] = Input_State::UP;
    input->keyboard[Key_Code::KP_ENTER] = Input_State::UP;
    input->keyboard[Key_Code::KP_EQUAL] = Input_State::UP;
    input->keyboard[Key_Code::LEFT_SHIFT] = Input_State::UP;
    input->keyboard[Key_Code::LEFT_CONTROL] = Input_State::UP;
    input->keyboard[Key_Code::LEFT_ALT] = Input_State::UP;
    input->keyboard[Key_Code::LEFT_SUPER] = Input_State::UP;
    input->keyboard[Key_Code::RIGHT_SHIFT] = Input_State::UP;
    input->keyboard[Key_Code::RIGHT_CONTROL] = Input_State::UP;
    input->keyboard[Key_Code::RIGHT_ALT] = Input_State::UP;
    input->keyboard[Key_Code::RIGHT_SUPER] = Input_State::UP;
    input->keyboard[Key_Code::MENU] = Input_State::UP;
    input->keyboard[Key_Code::LAST] = Input_State::UP;

    input->mouse.buttons[Mouse_Code::ONE] = Input_State::UP;
    input->mouse.buttons[Mouse_Code::TWO] = Input_State::UP;
    input->mouse.buttons[Mouse_Code::THREE] = Input_State::UP;
    input->mouse.buttons[Mouse_Code::FOUR] = Input_State::UP;
    input->mouse.buttons[Mouse_Code::FIVE] = Input_State::UP;
    input->mouse.buttons[Mouse_Code::SIX] = Input_State::UP;
    input->mouse.buttons[Mouse_Code::SEVEN] = Input_State::UP;
    input->mouse.buttons[Mouse_Code::EIGHT] = Input_State::UP;
    input->mouse.buttons[Mouse_Code::LAST] = Input_State::UP;
    input->mouse.buttons[Mouse_Code::LEFT] = Input_State::UP;
    input->mouse.buttons[Mouse_Code::RIGHT] = Input_State::UP;
    input->mouse.buttons[Mouse_Code::MIDDLE] = Input_State::UP;

    input->using_raw_input = false;
    input->cursor_visibility = Cursor_Visibility::NORMAL;

    return true;
}

Input_Callback_Info add_key_press_callback(Input *input, Key_Code key, std::function<void()> func) {
    input->key_press_callbacks[key][input->key_press_id] = func;

    Input_Callback_Info info = {};

    info.type = Input_Callback_Type::KEY;
    info.key = key;
    info.action = Input_State::DOWN;
    info.id = input->key_press_id;

    input->key_press_id++;

    return info;
}

Input_Callback_Info add_key_release_callback(Input *input, Key_Code key, std::function<void()> func) {
    input->key_release_callbacks[key][input->key_release_id] = func;

    Input_Callback_Info info = {};

    info.type = Input_Callback_Type::KEY;
    info.key = key;
    info.action = Input_State::UP;
    info.id = input->key_release_id;

    input->key_release_id++;

    return info;
}

Input_Callback_Info add_any_key_press_callback(Input *input, std::function<void(Key_Code)> func) {
    input->any_key_press_callbacks[input->any_key_press_id] = func;

    Input_Callback_Info info = {};

    info.type = Input_Callback_Type::KEY;
    info.action = Input_State::DOWN;
    info.is_any_code = true;
    info.id = input->any_key_press_id;

    input->any_key_press_id++;

    return info;
}

Input_Callback_Info add_any_key_release_callback(Input *input, std::function<void(Key_Code)> func) {
    input->any_key_release_callbacks[input->any_key_release_id] = func;

    Input_Callback_Info info = {};

    info.type = Input_Callback_Type::KEY;
    info.action = Input_State::UP;
    info.is_any_code = true;
    info.id = input->any_key_release_id;

    input->any_key_release_id++;

    return info;
}

Input_Callback_Info add_key_any_action_callback(Input *input, Key_Code key, std::function<void(Input_State)> func) {
    input->key_any_action_callbacks[key][input->key_any_action_id] = func;

    Input_Callback_Info info = {};

    info.type = Input_Callback_Type::KEY;
    info.is_any_code = true;
    info.key = key;
    info.id = input->key_any_action_id;

    input->key_any_action_id++;

    return info;
}

Input_Callback_Info add_any_key_any_action_callback(Input *input, std::function<void(Input_State, Key_Code)> func) {
    input->any_key_any_action_callbacks[input->any_key_any_action_id] = func;

    Input_Callback_Info info = {};

    info.type = Input_Callback_Type::KEY;
    info.is_any_code = true;
    info.is_any_action = true;
    info.id = input->any_key_any_action_id;

    input->any_key_any_action_id++;

    return info;
}

Input_Callback_Info add_mouse_button_press_callback(Input *input, Mouse_Code button, std::function<void()> func) {
    input->mouse_button_press_callbacks[button][input->mouse_button_press_id] = func;

    Input_Callback_Info info = {};

    info.type = Input_Callback_Type::MOUSE_BUTTON;
    info.button = button;
    info.action = Input_State::DOWN;
    info.id = input->mouse_button_press_id;

    input->mouse_button_press_id++;

    return info;
}

Input_Callback_Info add_mouse_button_release_callback(Input *input, Mouse_Code button, std::function<void()> func) {
    input->mouse_button_release_callbacks[button][input->mouse_button_release_id] = func;

    Input_Callback_Info info = {};

    info.type = Input_Callback_Type::MOUSE_BUTTON;
    info.button = button;
    info.action = Input_State::UP;
    info.id = input->mouse_button_release_id;

    input->mouse_button_release_id++;

    return info;
}

Input_Callback_Info add_any_mouse_button_press_callback(Input *input, std::function<void(Mouse_Code)> func) {
    input->any_mouse_button_press_callbacks[input->any_mouse_button_press_id] = func;

    Input_Callback_Info info = {};

    info.type = Input_Callback_Type::MOUSE_BUTTON;
    info.action = Input_State::DOWN;
    info.is_any_code = true;
    info.id = input->any_mouse_button_press_id;

    input->any_mouse_button_press_id++;

    return info;
}

Input_Callback_Info add_any_mouse_button_release_callback(Input *input, std::function<void(Mouse_Code)> func) {
    input->any_mouse_button_release_callbacks[input->any_mouse_button_release_id] = func;

    Input_Callback_Info info = {};

    info.type = Input_Callback_Type::MOUSE_BUTTON;
    info.action = Input_State::UP;
    info.is_any_code = true;
    info.id = input->any_mouse_button_release_id;

    input->any_mouse_button_release_id++;

    return info;
}

Input_Callback_Info add_mouse_button_any_action_callback(Input *input, Mouse_Code button, std::function<void(Input_State)> func) {
    input->mouse_button_any_action_callbakcs[button][input->mouse_button_any_action_id] = func;

    Input_Callback_Info info = {};

    info.type = Input_Callback_Type::MOUSE_BUTTON;
    info.is_any_action = true;
    info.button = button;
    info.id = input->mouse_button_any_action_id;

    input->mouse_button_any_action_id++;

    return info;
}

Input_Callback_Info add_any_mouse_button_any_action_callback(Input *input, std::function<void(Input_State, Mouse_Code)> func) {
    input->any_mouse_button_any_action_callbacks[input->any_mouse_button_any_action_id] = func;

    Input_Callback_Info info = {};

    info.type = Input_Callback_Type::MOUSE_BUTTON;
    info.is_any_action = true;
    info.is_any_code = true;
    info.id = input->any_mouse_button_any_action_id;

    input->any_mouse_button_any_action_id++;

    return info;
}

Input_Callback_Info add_mouse_move_callback(Input *input, std::function<void(f32, f32)> func) {
    input->mouse_move_callbacks[input->mouse_move_id] = func;

    Input_Callback_Info info = {};

    info.type = Input_Callback_Type::MOUSE_MOVE;
    info.id = input->mouse_move_id;

    input->mouse_move_id++;

    return info;
}

void SetKey(Input *input, Key_Code key, Input_State state) {
    input->keyboard[key] = state;

    switch (state) {
    case Input_State::DOWN:
        for (auto &it : input->key_press_callbacks[key]) {
            it.second();
        }
        for (auto &It : input->any_key_press_callbacks) {
            It.second(key);
        }
        break;
    case Input_State::UP:
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

void SetMouseButton(Input *input, Mouse_Code button, Input_State state) {
    input->mouse.buttons[button] = state;

    switch (state) {
    case Input_State::DOWN:
        for (auto &it : input->mouse_button_press_callbacks[button]) {
            it.second();
        }
        for (auto &it : input->any_mouse_button_press_callbacks) {
            it.second(button);
        }
        break;
    case Input_State::UP:
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

void RemoveCallback(Input *input, const Input_Callback_Info &info) {
    switch (info.type) {
    case Input_Callback_Type::KEY:
        if (info.is_any_action && info.is_any_code) {
            input->any_key_any_action_callbacks.erase(info.id);
        } else if (info.is_any_action) {
            input->key_any_action_callbacks[info.key].erase(info.id);
        } else if (info.is_any_code) {
            switch (info.action) {
            case Input_State::UP:
                input->any_key_release_callbacks.erase(info.id);
                break;
            case Input_State::DOWN:
                input->any_key_press_callbacks.erase(info.id);
                break;
            }
        } else {
            switch (info.action) {
            case Input_State::UP:
                input->key_release_callbacks[info.key].erase(info.id);
                break;
            case Input_State::DOWN:
                input->key_press_callbacks[info.key].erase(info.id);
                break;
            }
        }
        break;
    case Input_Callback_Type::MOUSE_BUTTON:
        if (info.is_any_action && info.is_any_code) {
            input->any_mouse_button_any_action_callbacks.erase(info.id);
        } else if (info.is_any_action) {
            input->mouse_button_any_action_callbakcs[info.button].erase(info.id);
        } else if (info.is_any_code) {
            switch (info.action) {
            case Input_State::UP:
                input->any_mouse_button_release_callbacks.erase(info.id);
                break;
            case Input_State::DOWN:
                input->any_mouse_button_press_callbacks.erase(info.id);
                break;
            }
        } else {
            switch (info.action) {
            case Input_State::UP:
                input->mouse_button_release_callbacks[info.button].erase(info.id);
                break;
            case Input_State::DOWN:
                input->mouse_button_press_callbacks[info.button].erase(info.id);
                break;
            }
        }
        break;
    case Input_Callback_Type::MOUSE_MOVE:
        input->mouse_move_callbacks.erase(info.id);
        break;
    }
}

void set_raw_input(Input *input, bool value) {
    if (input->using_raw_input != value) {
        input->using_raw_input = value;
        if (value) {
            if (input->cursor_visibility == Cursor_Visibility::DISABLED) {
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

void set_cursor_visibility(Input *input, Cursor_Visibility visibility) {
    if (visibility != input->cursor_visibility) {
        input->cursor_visibility = visibility;
        if (input->cursor_visibility != Cursor_Visibility::DISABLED && input->using_raw_input) {
            SK_LOG_ERROR("Cannot change cursor state from disabled while raw input is enabled");
            return;
        }
        platform_set_cursor_visibility(input->window, visibility);
    }
}
