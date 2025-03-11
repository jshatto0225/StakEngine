#pragma once

#include "Types.h"
#include "Asserts.h"

#include <unordered_map>
#include <functional>

#undef DELETE

struct IWindow;

enum class EKeyCode : FSInt16 {
    UNKNOWN       = -1,
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

enum class EMouseCode : FUInt8 {
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

enum class ECursorVisibility : FUInt8 {
    NORMAL,
    HIDDEN,
    DISABLED
};

enum class EInputState : FUInt8 {
    DOWN,
    UP,
};

enum class ECallbackType {
    KEY,
    MOUSE_BUTTON,
    MOUSE_MOVE,
};

struct FInputCallbackInfo {
    ECallbackType Type;
    
    bool IsAnyCode;
    bool IsAnyAction;

    EKeyCode Key;
    EMouseCode Button;

    EInputState Action;

    FUInt64 Id;
};

struct FInput {
    FInput(TRef<IWindow> window);

    struct FMousePosData {
        FFloat X;
        FFloat Y;
    };

    inline const FMousePosData &GetMousePos() const { return MousePos; }
    inline FFloat GetMouseX() const { return GetMousePos().X; }
    inline FFloat GetMouseY() const { return GetMousePos().Y; }
    inline EInputState GetKey(EKeyCode Key) const { return Keys.at(Key); }
    inline EInputState GetMouseButton(EMouseCode Button) const { return MouseButtons.at(Button); }

    inline bool IsUsingRawInput() { return UsingRawInput; }
    void SetRawInput(FBool Value);

    inline ECursorVisibility GetCursorVisibility() { return CursorVisibility; }
    void SetCursorVisibility(ECursorVisibility Visibility);

    FInputCallbackInfo AddKeyPressCallback(EKeyCode Key, std::function<void()> Func);
    FInputCallbackInfo AddKeyReleaseCallback(EKeyCode Key, std::function<void()> Func);

    FInputCallbackInfo AddAnyKeyPressCallback(std::function<void(EKeyCode)> Func);
    FInputCallbackInfo AddAnyKeyReleaseCallback(std::function<void(EKeyCode)> Func);

    FInputCallbackInfo AddKeyAnyActionCallback(EKeyCode Key, std::function<void(EInputState)> Func);
    FInputCallbackInfo AddAnyKeyAnyActionCallback(std::function<void(EInputState, EKeyCode)> Func);
  
    FInputCallbackInfo AddMouseButtonPressCallback(EMouseCode Button, std::function<void()> Func);
    FInputCallbackInfo AddMouseButtonReleaseCallback(EMouseCode Button, std::function<void()> Func);

    FInputCallbackInfo AddAnyMouseButtonPressCallback(std::function<void(EMouseCode)> Func);
    FInputCallbackInfo AddAnyMouseButtonReleaseCallback(std::function<void(EMouseCode)> Func);

    FInputCallbackInfo AddMouseButtonAnyActionCallback(EMouseCode Button, std::function<void(EInputState)> Func);
    FInputCallbackInfo AddAnyMouseButtonAnyActionCallback(std::function<void(EInputState, EMouseCode)> Func);
  
    FInputCallbackInfo AddMouseMoveCallback(std::function<void(FFloat, FFloat)> Func);

    void RemoveCallback(const FInputCallbackInfo &Info);

    void SetKey(EKeyCode Key, EInputState Action);
    void SetMouseButton(EMouseCode Button, EInputState Action);
    void SetMousePos(FFloat X, FFloat Y);

    FBool UsingRawInput;
    ECursorVisibility CursorVisibility;

    FMousePosData MousePos = {};
    std::unordered_map<EKeyCode, EInputState> Keys;
    std::unordered_map<EMouseCode, EInputState> MouseButtons;

    FUInt64 KeyPressId = 0;
    std::unordered_map<EKeyCode, std::unordered_map<FUInt64, std::function<void()>>> KeyPressCallbacks;
    FUInt64 KeyReleaseId = 0;
    std::unordered_map<EKeyCode, std::unordered_map<FUInt64, std::function<void()>>> KeyReleaseCallbacks;

    FUInt64 AnyKeyPressId = 0;
    std::unordered_map<FUInt64, std::function<void(EKeyCode)>> AnyKeyPressCallbacks;
    FUInt64 AnyKeyReleaseId = 0;
    std::unordered_map<FUInt64, std::function<void(EKeyCode)>> AnyKeyReleaseCallbacks;

    FUInt64 KeyAnyActionId = 0;
    std::unordered_map<EKeyCode, std::unordered_map<FUInt64, std::function<void(EInputState)>>> KeyAnyActionCallbacks;
    FUInt64 AnyKeyAnyActionId = 0;
    std::unordered_map<FUInt64, std::function<void(EInputState, EKeyCode)>> AnyKeyAnyActionCallbacks;

    FUInt64 MouseButtonPressId = 0;
    std::unordered_map<EMouseCode, std::unordered_map<FUInt64, std::function<void()>>> MouseButtonPressCallbacks;
    FUInt64 MouseButtonReleaseId = 0;
    std::unordered_map<EMouseCode, std::unordered_map<FUInt64, std::function<void()>>> MouseButtonReleaseCallbacks;

    FUInt64 AnyMouseButtonPressId = 0;
    std::unordered_map<FUInt64, std::function<void(EMouseCode)>> AnyMouseButtonPressCallbacks;
    FUInt64 AnyMouseButtonReleaseId = 0;
    std::unordered_map<FUInt64, std::function<void(EMouseCode)>> AnyMouseButtonReleaseCallbacks;

    FUInt64 MouseButtonAnyActionId = 0;
    std::unordered_map<EMouseCode, std::unordered_map<FUInt64, std::function<void(EInputState)>>> MouseButtonAnyActionCallbacks;
    FUInt64 AnyMouseButtonAnyActionId = 0;
    std::unordered_map<FUInt64, std::function<void(EInputState, EMouseCode)>> AnyMouseButtonAnyActionCallbacks;

    FUInt64 MouseMoveId = 0;
    std::unordered_map<FUInt64, std::function<void(FFloat, FFloat)>> MouseMoveCallbacks;

    TRef<IWindow> Window;
};
