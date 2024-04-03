#pragma once

#include "Types.h"
#include "StakMath.h"
#include "Renderer.h"
#include "Texture.h"

void Renderer2DInit();
void Renderer2DBeginScene(const scene_view_camera *Cam);
void Renderer2DEndScene();
void Renderer2DShutdown();

void Renderer2DDrawCircle(const vec2 *Pos, const vec2 *Size, const vec4 *Color);
void Renderer2DDrawCircle(const vec2 *Pos, const vec2 *Size, texture2d **Tex, bool TransferTextureOwnership);
void Renderer2DDrawCircle(const vec2 *Pos, const vec2 *Size, f32 Rotation, const vec4 *Color);
void Renderer2DDrawCircle(const vec2 *Pos, const vec2 *Size, f32 Rotation, texture2d **Tex, bool TransferTextrureOwnership);
void Renderer2DDrawCircle(const mat4 *Transform, texture2d **Tex, bool TransferTextureOwnership);

void Renderer2DDrawQuad(const vec2 *Pos, const vec2 *Size, const vec4 *Color);
void Renderer2DDrawQuad(const vec2 *Pos, const vec2 *Size, texture2d **Tex, bool TransferTextureOwnership);
void Renderer2DDrawQuad(const vec2 *Pos, const vec2 *Size, f32 Rotation, const vec4 *Color);
void Renderer2DDrawQuad(const vec2 *Pos, const vec2 *Size, f32 Rotation, texture2d **Tex, bool TransferTextureOwnership);
void Renderer2DDrawQuad(const mat4 *Transform, texture2d **Tex, bool TransferTextureOwnership);
