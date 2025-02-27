#include "input.hpp"

#include "log.hpp"
#include "platform.hpp"
#include "utils.hpp"
#include "window.hpp"

#undef DELETE

static auto raw_input = false;
static auto cursor_visibility = Cursor_Visibility::NORMAL;

static f32 mouse_x;
static f32 mouse_y;

static Hash_Map<Key_Code, Input_State> keys = {
    { Key_Code::UNKNOWN,    Input_State::UP },
    { Key_Code::SPACE,      Input_State::UP },
    { Key_Code::APOSTROPHE, Input_State::UP },
    { Key_Code::COMMA,      Input_State::UP },
    { Key_Code::MINUS,      Input_State::UP },
    { Key_Code::PERIOD,     Input_State::UP },
    { Key_Code::SLASH,      Input_State::UP },
    { Key_Code::ZERO,       Input_State::UP },
    { Key_Code::ONE,        Input_State::UP },
    { Key_Code::TWO,        Input_State::UP },
    { Key_Code::THREE,      Input_State::UP },
    { Key_Code::FOUR,       Input_State::UP },
    { Key_Code::FIVE,       Input_State::UP },
    { Key_Code::SIX,        Input_State::UP },
    { Key_Code::SEVEN,      Input_State::UP },
    { Key_Code::EIGHT,      Input_State::UP },
    { Key_Code::NINE,       Input_State::UP },
    { Key_Code::SEMICOLON,  Input_State::UP },
    { Key_Code::EQUAL,      Input_State::UP },
    
    { Key_Code::A, Input_State::UP },
    { Key_Code::B, Input_State::UP },
    { Key_Code::C, Input_State::UP },
    { Key_Code::D, Input_State::UP },
    { Key_Code::E, Input_State::UP },
    { Key_Code::F, Input_State::UP },
    { Key_Code::G, Input_State::UP },
    { Key_Code::H, Input_State::UP },
    { Key_Code::I, Input_State::UP },
    { Key_Code::J, Input_State::UP },
    { Key_Code::K, Input_State::UP },
    { Key_Code::L, Input_State::UP },
    { Key_Code::M, Input_State::UP },
    { Key_Code::N, Input_State::UP },
    { Key_Code::O, Input_State::UP },
    { Key_Code::P, Input_State::UP },
    { Key_Code::Q, Input_State::UP },
    { Key_Code::R, Input_State::UP },
    { Key_Code::S, Input_State::UP },
    { Key_Code::T, Input_State::UP },
    { Key_Code::U, Input_State::UP },
    { Key_Code::V, Input_State::UP },
    { Key_Code::W, Input_State::UP },
    { Key_Code::X, Input_State::UP },
    { Key_Code::Y, Input_State::UP },
    { Key_Code::Z, Input_State::UP },
    
    { Key_Code::LEFT_BRACKET,  Input_State::UP },
    { Key_Code::BACKSLASH,     Input_State::UP },
    { Key_Code::RIGHT_BRACKET, Input_State::UP },
    { Key_Code::GRAVE_ACCENT,  Input_State::UP },
    { Key_Code::WORLD_1,       Input_State::UP },
    { Key_Code::WORLD_2,       Input_State::UP },
    { Key_Code::ESCAPE,        Input_State::UP },
    { Key_Code::ENTER,         Input_State::UP },
    { Key_Code::TAB,           Input_State::UP },
    { Key_Code::BACKSPACE,     Input_State::UP },
    { Key_Code::INSERT,        Input_State::UP },
    { Key_Code::DELETE,        Input_State::UP },
    
    { Key_Code::RIGHT,         Input_State::UP },
    { Key_Code::LEFT,          Input_State::UP },
    { Key_Code::DOWN,          Input_State::UP },
    { Key_Code::UP,            Input_State::UP },
    
    { Key_Code::PAGE_UP,      Input_State::UP },
    { Key_Code::PAGE_DOWN,    Input_State::UP },
    { Key_Code::HOME,         Input_State::UP },
    { Key_Code::END,          Input_State::UP },
    { Key_Code::CAPS_LOCK,    Input_State::UP },
    { Key_Code::SCROLL_LOCK,  Input_State::UP },
    { Key_Code::NUM_LOCK,     Input_State::UP },
    { Key_Code::PRINT_SCREEN, Input_State::UP },
    { Key_Code::PAUSE,        Input_State::UP },
    
    { Key_Code::F1,   Input_State::UP },
    { Key_Code::F2,   Input_State::UP },
    { Key_Code::F3,   Input_State::UP },
    { Key_Code::F4,   Input_State::UP },
    { Key_Code::F5,   Input_State::UP },
    { Key_Code::F6,   Input_State::UP },
    { Key_Code::F7,   Input_State::UP },
    { Key_Code::F8,   Input_State::UP },
    { Key_Code::F9,   Input_State::UP },
    { Key_Code::F10,  Input_State::UP },
    { Key_Code::F11,  Input_State::UP },
    { Key_Code::F12,  Input_State::UP },
    { Key_Code::F13,  Input_State::UP },
    { Key_Code::F14,  Input_State::UP },
    { Key_Code::F15,  Input_State::UP },
    { Key_Code::F16,  Input_State::UP },
    { Key_Code::F17,  Input_State::UP },
    { Key_Code::F18,  Input_State::UP },
    { Key_Code::F19,  Input_State::UP },
    { Key_Code::F20,  Input_State::UP },
    { Key_Code::F21,  Input_State::UP },
    { Key_Code::F22,  Input_State::UP },
    { Key_Code::F23,  Input_State::UP },
    { Key_Code::F24,  Input_State::UP },
    { Key_Code::F25,  Input_State::UP },
    
    { Key_Code::KP_0, Input_State::UP },
    { Key_Code::KP_1, Input_State::UP },
    { Key_Code::KP_2, Input_State::UP },
    { Key_Code::KP_3, Input_State::UP },
    { Key_Code::KP_4, Input_State::UP },
    { Key_Code::KP_5, Input_State::UP },
    { Key_Code::KP_6, Input_State::UP },
    { Key_Code::KP_7, Input_State::UP },
    { Key_Code::KP_8, Input_State::UP },
    { Key_Code::KP_9, Input_State::UP },
    
    { Key_Code::KP_DECIMAL, Input_State::UP },
    { Key_Code::KP_DIVIDE,   Input_State::UP },
    { Key_Code::KP_MULTIPLY, Input_State::UP },
    { Key_Code::KP_SUBTRACT, Input_State::UP },
    { Key_Code::KP_ADD,      Input_State::UP },
    { Key_Code::KP_ENTER,    Input_State::UP },
    { Key_Code::KP_EQUAL,    Input_State::UP },
    
    { Key_Code::LEFT_SHIFT,    Input_State::UP },
    { Key_Code::LEFT_CONTROL,  Input_State::UP },
    { Key_Code::LEFT_ALT,      Input_State::UP },
    { Key_Code::LEFT_SUPER,    Input_State::UP },
    { Key_Code::RIGHT_SHIFT,   Input_State::UP },
    { Key_Code::RIGHT_CONTROL, Input_State::UP },
    { Key_Code::RIGHT_ALT,     Input_State::UP },
    { Key_Code::RIGHT_SUPER,   Input_State::UP },
    { Key_Code::MENU,          Input_State::UP },
    { Key_Code::LAST,          Input_State::UP },

};

static Hash_Map<Mouse_Code, Input_State> mouse_buttons = {
    { Mouse_Code::ONE,    Input_State::UP },
    { Mouse_Code::TWO,    Input_State::UP },
    { Mouse_Code::THREE,  Input_State::UP },
    { Mouse_Code::FOUR,   Input_State::UP },
    { Mouse_Code::FIVE,   Input_State::UP },
    { Mouse_Code::SIX,    Input_State::UP },
    { Mouse_Code::SEVEN,  Input_State::UP },
    { Mouse_Code::EIGHT,  Input_State::UP },
    { Mouse_Code::LAST,   Input_State::UP },
    { Mouse_Code::LEFT,   Input_State::UP },
    { Mouse_Code::RIGHT,  Input_State::UP },
    { Mouse_Code::MIDDLE, Input_State::UP },
};

static Hash_Map<Key_Code, Array<void(*)()>> key_press_callbacks;
static Hash_Map<Key_Code, Array<void(*)()>> key_release_callbacks;

static Array<void(*)(Key_Code)> generic_key_press_callbacks;
static Array<void(*)(Key_Code)> generic_key_release_callbacks;

static Array<void(*)(Input_State, Key_Code)> generic_key_callbacks;

static Hash_Map<Mouse_Code, Array<void(*)()>> mouse_button_pressed_callbacks;
static Hash_Map<Mouse_Code, Array<void(*)()>> mouse_button_released_callbacks;

static Array<void(*)(Mouse_Code)> generic_mouse_button_press_callbacks;
static Array<void(*)(Mouse_Code)> generic_mouse_button_release_callbacks;

static Array<void(*)(Input_State, Mouse_Code)> generic_mouse_button_callbacks;

static Array<void(*)(f32, f32)> mouse_move_callbacks;

void get_mouse_pos(f32* x, f32* y) {
    *x = mouse_x;
    *y = mouse_y;
}

f32 get_mouse_x() {
    return mouse_x;
}

f32 get_mouse_y() {
    return mouse_y;
}

Input_State get_key(Key_Code key) {
    return keys[key];
}
    
Input_State get_mouse_button(Mouse_Code button) {
    return mouse_buttons[button];
}

void add_key_state_callback(Input_State state, Key_Code key, void(*func)()) {
    switch (state) {
    case Input_State::DOWN:
        key_press_callbacks[key].push_back(func);
        break;
    case Input_State::UP:
        key_release_callbacks[key].push_back(func);
        break;
    }
}

void add_generic_key_state_callback(Input_State state, void(*func)(Key_Code)) {
    switch (state) {
    case Input_State::DOWN:
        generic_key_press_callbacks.push_back(func);
        break;
    case Input_State::UP:
        generic_key_release_callbacks.push_back(func);
        break;
    }
}

void add_generic_key_callback(void(*func)(Input_State, Key_Code)) {
    generic_key_callbacks.push_back(func);
}

void add_mouse_button_state_callback(Input_State state, Mouse_Code button, void(*func)()) {
    switch (state) {
    case Input_State::DOWN:
        mouse_button_pressed_callbacks[button].push_back(func);
        break;
    case Input_State::UP:
        mouse_button_released_callbacks[button].push_back(func);
        break;
    }
}

void add_generic_mouse_button_state_callback(Input_State state, void(*func)(Mouse_Code)) {
    switch (state) {
    case Input_State::DOWN:
        generic_mouse_button_press_callbacks.push_back(func);
        break;
    case Input_State::UP:
        generic_mouse_button_release_callbacks.push_back(func);
        break;
    }
}

void add_generic_mouse_button_callback(void(*func)(Input_State, Mouse_Code)) {
    generic_mouse_button_callbacks.push_back(func);
}

void add_mouse_move_callback(void(*func)(f32, f32)) {
    mouse_move_callbacks.push_back(func);
}

void set_key(Key_Code key, Input_State state) {
    keys[key] = state;

    switch (state) {
    case Input_State::DOWN:
        It (key_press_callbacks[key]) {
            it();
        }
        It (generic_key_press_callbacks) {
            it(key);
        }
        break;
    case Input_State::UP:
        It (key_release_callbacks[key]) {
            it();
        }
        It (generic_key_release_callbacks) {
            it(key);
        }
        break;
    }

    It (generic_key_callbacks) {
        it(state, key);
    }
}

void set_mouse_button(Mouse_Code button, Input_State state) {
    mouse_buttons[button] = state;

    switch (state) {
    case Input_State::DOWN:
        It (mouse_button_pressed_callbacks[button]) {
            it();
        }
        It (generic_mouse_button_press_callbacks) {
            it(button);
        }
        break;
    case Input_State::UP:
        It (mouse_button_released_callbacks[button]) {
            it();
        }
        It (generic_mouse_button_release_callbacks) {
            it(button);
        }
        break;
    }
    
    It (generic_mouse_button_callbacks) {
        it(state, button);
    }
}

void set_mouse_pos(f32 x, f32 y) {
    mouse_x = x;
    mouse_y = y;

    It (mouse_move_callbacks) {
        it(x, y);
    }
}

void set_raw_input(Ref<Window> window, bool value) {
    if (raw_input != value) {
        raw_input = value;
        if (value) {
            if (cursor_visibility == Cursor_Visibility::DISABLED) {
                window->enable_raw_input();
            }
            else {
                SK_LOG_WARN("Cannot enable raw input while cursor is not disabled");
            }
        }
        else {
            window->disable_raw_input();
        }
    }
}

void set_cursor_visibility(Ref<Window> window, Cursor_Visibility visibility) {
    if (visibility != cursor_visibility) {
        cursor_visibility = visibility;
        if (cursor_visibility != Cursor_Visibility::DISABLED && raw_input) {
            SK_LOG_WARN("Cannot change cursor state from disabled while raw input is enabled");
            return;
        }
        
        if (visibility == Cursor_Visibility::DISABLED) {
            window->hide_cursor();
        } else {
            window->show_cursor();
        }
    }
}
