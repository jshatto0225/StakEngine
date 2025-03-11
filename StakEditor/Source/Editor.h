#pragma once

#include <StakEngine.h>

bool Init(FEngine *Engine, FApplication *App);
void Shutdown(FEngine *Engine, FApplication *App);
void Update(FEngine *Engine, FApplication *App);
void OnImGuiRender(FEngine *Engine, FApplication *App);

struct FEditor {
    FInputCallbackInfo EscapePressedCallbackInfo;
    FInputCallbackInfo WPressedCallbackInfo;
};