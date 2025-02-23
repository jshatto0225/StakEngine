#pragma once

#include "Types.h"
#include "Asserts.h"

#include <unordered_map>
#include <functional>

#undef DELETE

enum class Key_Code : s16 {
    UNKNOWN = -1,
    SPACE         = 32,
    APOSTROPHE    = 39, /* ' */
    COMMA         = 44, /* , */
    MINUS         = 45, /* - */
    PERIOD        = 46, /* . */
    SLASH         = 47, /* / */
    ZERO          = 48,
    ONE           = 49,
    TWO           = 50,
    THREE         = 51,
    FOUR          = 52,
    FIVE          = 53,
    SIX           = 54,
    SEVEN         = 55,
    EIGHT         = 56,
    NINE          = 57,
    SEMICOLON     = 59, /* ; */
    EQUAL         = 61, /* = */
    A             = 65,
    B             = 66,
    C             = 67,
    D             = 68,
    E             = 69,
    F             = 70,
    G             = 71,
    H             = 72,
    I             = 73,
    J             = 74,
    K             = 75,
    L             = 76,
    M             = 77,
    N             = 78,
    O             = 79,
    P             = 80,
    Q             = 81,
    R             = 82,
    S             = 83,
    T             = 84,
    U             = 85,
    V             = 86,
    W             = 87,
    X             = 88,
    Y             = 89,
    Z             = 90,
    LEFT_BRACKET  = 91,/* [ */
    BACKSLASH     = 92,/* \ */
    RIGHT_BRACKET = 93,/* ] */
    GRAVE_ACCENT  = 96,/* ` */
    WORLD_1       = 161, /* non-US #1 */
    WORLD_2       = 162, /* non-US #2 */
    ESCAPE        = 256,
    ENTER         = 257,
    TAB           = 258,
    BACKSPACE     = 259,
    INSERT        = 260,
    DELETE        = 261,
    RIGHT         = 262,
    LEFT          = 263,
    DOWN          = 264,
    UP            = 265,
    PAGE_UP       = 266,
    PAGE_DOWN     = 267,
    HOME          = 268,
    END           = 269,
    CAPS_LOCK     = 280,
    SCROLL_LOCK   = 281,
    NUM_LOCK      = 282,
    PRINT_SCREEN  = 283,
    PAUSE         = 284,
    F1            = 290,
    F2            = 291,
    F3            = 292,
    F4            = 293,
    F5            = 294,
    F6            = 295,
    F7            = 296,
    F8            = 297,
    F9            = 298,
    F10           = 299,
    F11           = 300,
    F12           = 301,
    F13           = 302,
    F14           = 303,
    F15           = 304,
    F16           = 305,
    F17           = 306,
    F18           = 307,
    F19           = 308,
    F20           = 309,
    F21           = 310,
    F22           = 311,
    F23           = 312,
    F24           = 313,
    F25           = 314,
    KP_0          = 320,
    KP_1          = 321,
    KP_2          = 322,
    KP_3          = 323,
    KP_4          = 324,
    KP_5          = 325,
    KP_6          = 326,
    KP_7          = 327,
    KP_8          = 328,
    KP_9          = 329,
    KP_DECIMAL    = 330,
    KP_DIVIDE     = 331,
    KP_MULTIPLY   = 332,
    KP_SUBTRACT   = 333,
    KP_ADD        = 334,
    KP_ENTER      = 335,
    KP_EQUAL      = 336,
    LEFT_SHIFT    = 340,
    LEFT_CONTROL  = 341,
    LEFT_ALT      = 342,
    LEFT_SUPER    = 343,
    RIGHT_SHIFT   = 344,
    RIGHT_CONTROL = 345,
    RIGHT_ALT     = 346,
    RIGHT_SUPER   = 347,
    MENU          = 348,
    LAST          = MENU
};

enum class Mouse_Code : u8 {
    ONE     = 0,
    TWO     = 1,
    THREE   = 2,
    FOUR    = 3,
    FIVE    = 4,
    SIX     = 5,
    SEVEN   = 6,
    EIGHT   = 7,
    LAST    = EIGHT,
    LEFT    = ONE,
    RIGHT   = TWO,
    MIDDLE  = THREE
};

enum class Cursor_Visibility : u8 {
    NORMAL,
    HIDDEN,
    DISABLED
};

enum class Input_State : u8 {
    DOWN,
    UP,
};

class Input {
public:
    Input();

    ~Input();

    inline static Input &get() { ASSERT(instance); return *instance; }

    inline void get_mouse_pos(f32* x, f32* y) const { *x = mouse_x; *y = mouse_y; }
    inline f32 get_mouse_x() const { return mouse_x; }
    inline f32 get_mouse_y() const { return mouse_y; }
    inline Input_State get_key(Key_Code key) const { return keys.at(key); }
    inline Input_State get_mouse_button(Mouse_Code button) const { return mouse_buttons.at(button); }

    inline bool is_using_raw_input() { return raw_input; }
    void set_raw_input(bool value);

    inline Cursor_Visibility get_cursor_visibility() { return cursor_visibility; }
    void set_cursor_visibility(Cursor_Visibility visibility);

    void add_key_state_callback(Input_State action, Key_Code key, std::function<void()> func);
    void add_generic_key_state_callback(Input_State action, std::function<void(Key_Code)> func);
    void add_generic_key_callback(std::function<void(Input_State, Key_Code)> func);
  
    void add_mouse_button_state_callback(Input_State action, Mouse_Code button, std::function<void()> func);
    void add_generic_mouse_button_state_callback(Input_State action, std::function<void(Mouse_Code)> func);
    void add_generic_mouse_button_callback(std::function<void(Input_State, Mouse_Code)> func);
  
    void add_mouse_move_callback(std::function<void(f32, f32)> Func);

private:
    void set_key(Key_Code Key, Input_State Action);
    void set_mouse_button(Mouse_Code Button, Input_State Action);
    void set_mouse_pos(f32 X, f32 Y);

private:
    inline static Input *instance = nullptr;

private:
    bool raw_input;
    Cursor_Visibility cursor_visibility;

    f32 mouse_x;
    f32 mouse_y;
    std::unordered_map<Key_Code, Input_State> keys;
    std::unordered_map<Mouse_Code, Input_State> mouse_buttons;

    std::unordered_map<Key_Code, std::vector<std::function<void()>>> key_press_callbacks;
    std::unordered_map<Key_Code, std::vector<std::function<void()>>> key_release_callbacks;

    std::vector<std::function<void(Key_Code)>> generic_key_press_callbacks;
    std::vector<std::function<void(Key_Code)>> generic_key_release_callbacks;

    std::vector<std::function<void(Input_State, Key_Code)>> generic_key_callbacks;

    std::unordered_map<Mouse_Code, std::vector<std::function<void()>>> mouse_button_pressed_callbacks;
    std::unordered_map<Mouse_Code, std::vector<std::function<void()>>> mouse_button_released_callbacks;

    std::vector<std::function<void(Mouse_Code)>> generic_mouse_button_press_callbacks;
    std::vector<std::function<void(Mouse_Code)>> generic_mouse_button_release_callbacks;

    std::vector<std::function<void(Input_State, Mouse_Code)>> generic_mouse_button_callbacks;

    std::vector<std::function<void(f32, f32)>> mouse_move_callbacks;

private:
    friend class App;
};
