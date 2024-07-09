#pragma once

#include "../StakEngine/StakEngine.h"

namespace EditorLayer
{
void Init();
void Shutdown();
void Update();
void OnEvent(const event *Event);
}
