#include "InputManager.h"

class Win32InputManager : public InputManager
{
public:
    bool KeyDown(i32 key) override;
    MousePosition GetMousePosition() override;
};