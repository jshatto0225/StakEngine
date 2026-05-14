#pragma once

#include "Types.h"
#include "Asserts.h"

#include <functional>

#include "InputEnums.h"
#include "Window.h"

struct InputCallbackInfo {
    InputCallbackType type;
    
    bool is_any_code;
    bool is_any_action;

    KeyCode key;
    MouseCode button;

    InputState action;

    u64 id;
};

struct Input {
    struct {
        f32 x;
        f32 y;

        std::unordered_map<MouseCode, InputState> buttons;
    } mouse;
    std::unordered_map<KeyCode, InputState> keyboard;

    bool using_raw_input;
    CursorVisibility cursor_visibility;

    u64 key_press_id = 0;
    std::unordered_map<KeyCode, std::unordered_map<u64, std::function<void()>>> key_press_callbacks;
    u64 key_release_id = 0;
    std::unordered_map<KeyCode, std::unordered_map<u64, std::function<void()>>> key_release_callbacks;

    u64 any_key_press_id = 0;
    std::unordered_map<u64, std::function<void(KeyCode)>> any_key_press_callbacks;
    u64 any_key_release_id = 0;
    std::unordered_map<u64, std::function<void(KeyCode)>> any_key_release_callbacks;

    u64 key_any_action_id = 0;
    std::unordered_map<KeyCode, std::unordered_map<u64, std::function<void(InputState)>>> key_any_action_callbacks;
    u64 any_key_any_action_id = 0;
    std::unordered_map<u64, std::function<void(InputState, KeyCode)>> any_key_any_action_callbacks;

    u64 mouse_button_press_id = 0;
    std::unordered_map<MouseCode, std::unordered_map<u64, std::function<void()>>> mouse_button_press_callbacks;
    u64 mouse_button_release_id = 0;
    std::unordered_map<MouseCode, std::unordered_map<u64, std::function<void()>>> mouse_button_release_callbacks;

    u64 any_mouse_button_press_id = 0;
    std::unordered_map<u64, std::function<void(MouseCode)>> any_mouse_button_press_callbacks;
    u64 any_mouse_button_release_id = 0;
    std::unordered_map<u64, std::function<void(MouseCode)>> any_mouse_button_release_callbacks;

    u64 mouse_button_any_action_id = 0;
    std::unordered_map<MouseCode, std::unordered_map<u64, std::function<void(InputState)>>> mouse_button_any_action_callbakcs;
    u64 any_mouse_button_any_action_id = 0;
    std::unordered_map<u64, std::function<void(InputState, MouseCode)>> any_mouse_button_any_action_callbacks;

    u64 mouse_move_id = 0;
    std::unordered_map<u64, std::function<void(f32, f32)>> mouse_move_callbacks;

    Window window;
};

Input *create_input(Window window);
void destroy_input(Input *input);

void set_raw_input(Input *input, bool value);
void set_cursor_visibility(Input *input, CursorVisibility visibility);

InputCallbackInfo add_key_press_callback(Input *input, KeyCode key, std::function<void()> func);
InputCallbackInfo add_key_release_callback(Input *input, KeyCode key, std::function<void()> func);
InputCallbackInfo add_any_key_press_callback(Input *input, std::function<void(KeyCode)> func);
InputCallbackInfo add_any_key_release_callback(Input *input, std::function<void(KeyCode)> func);
InputCallbackInfo add_key_any_action_callback(Input *input, KeyCode key, std::function<void(InputState)> func);
InputCallbackInfo add_any_key_any_action_callback(Input *input, std::function<void(InputState, KeyCode)> func);
InputCallbackInfo add_mouse_button_press_callback(Input *input, MouseCode button, std::function<void()> func);
InputCallbackInfo add_mouse_button_release_callback(Input *input, MouseCode button, std::function<void()> func);
InputCallbackInfo add_any_mouse_button_press_callback(Input *input, std::function<void(MouseCode)> func);
InputCallbackInfo add_any_mouse_button_release_callback(Input *input, std::function<void(MouseCode)> func);
InputCallbackInfo add_mouse_button_any_action_callback(Input *input, MouseCode button, std::function<void(InputState)> func);
InputCallbackInfo add_any_mouse_button_any_action_callback(Input *input, std::function<void(InputState, MouseCode)> func);
InputCallbackInfo add_mouse_move_callback(Input *input, std::function<void(f32, f32)> func);

void remove_callback(Input *input, const InputCallbackInfo &info);
void set_key(Input *input, KeyCode key, InputState action);
void set_mouse_button(Input *input, MouseCode button, InputState action);
void set_mouse_pos(Input *input, f32 x, f32 y);
InputState get_key(Input *input, KeyCode key);