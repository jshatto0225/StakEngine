#pragma once

#include "Types.h"
#include "StakMath.h"

// Because of some wierd macro
#undef DELETE

/**
 * Platform agnostic key codes
 */
enum class Key {
  BUTTON_1,
  BUTTON_2,
  BUTTON_3,
  BUTTON_4,
  BUTTON_5,
  BUTTON_6,
  BUTTON_7,
  BUTTON_8,
  BUTTON_LEFT = BUTTON_1,
  BUTTON_RIGHT = BUTTON_2,
  BUTTON_MIDDLE = BUTTON_3,

  SPACE,
  APOSTROPHE,
  COMMA,
  MINUS,
  PERIOD,
  SLASH,
  ZERO,
  ONE,
  TWO,
  THREE,
  FOUR,
  FIVE,
  SIX,
  SEVEN,
  EIGHT,
  NINE,
  SEMICOLON,
  EQUAL,
  A,
  B,
  C,
  D,
  E,
  F,
  G,
  H,
  I,
  J,
  K,
  L,
  M,
  N,
  O,
  P,
  Q,
  R,
  S,
  T,
  U,
  V,
  W,
  X,
  Y,
  Z,
  LEFT_BRACKET,
  BACKSLASH,
  RIGHT_BRACKET,
  GRAVE,
  ESCAPE,
  ENTER,
  TAB,
  BACKSPACE,
  INSTERT,
  DELETE,
  RIGHT,
  LEFT,
  DOWN,
  UP,
  PAGE_UP,
  PAGE_DOWN,
  HOME,
  END,
  CAPS_LOCK,
  SCROLL_LOCK,
  NUM_LOCK,
  PRINT_SCREEN,
  PAUSE,
  F1,
  F2,
  F3,
  F4,
  F5,
  F6,
  F7,
  F8,
  F9,
  F10,
  F11,
  F12,
  F13,
  F14,
  F15,
  F16,
  F17,
  F18,
  F19,
  F20,
  F21,
  F22,
  F23,
  F24,
  F25,
  NUM_ZERO,
  NUM_ONE,
  NUM_TWO,
  NUM_THREE,
  NUM_FOUR,
  NUM_FIVE,
  NUM_SIX,
  NUM_SEVEN,
  NUM_EIGHT,
  NUM_NINE,
  NUM_DECIMAL,
  NUM_DIVIDE,
  NUM_MULTIPLY,
  NUM_SUBTRACT,
  NUM_ADD,
  NUM_ENTER,
  NUM_EQUAL,
  LEFT_SHIFT,
  LEFT_CONTROL,
  LEFT_ALT,
  LEFT_SUPER,
  RIGHT_SHIFT,
  RIGHT_CONTROL,
  RIGHT_ALT,
  RIGHT_SUPER,
  MENU
};

/**
 * Input Class
 * Not really a class, acts more like a namespace
 */
class Input {
public:
  static bool GetKeyDown(Key key);
  static Vec2 GetMousePos();
};

