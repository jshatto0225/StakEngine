#pragma once

#include "Window.h"
#include "Renderer.h"

void RenderApiInit(window *Window);
void RenderApiShutdown();
void RenderApiSwapBuffers();
void RenderApiSetClearColor(f32 Red, f32 Green, f32 Blue, f32 Alpha);
void RenderApiSetViewport(i32 X, i32 Y, i32 Width, i32 Height);
void RenderApiSetLineWidth(f32 Width);
void RenderApiBind();
void RenderApiClear();
void RenderApiDrawIndexed(const vertex_array *VertexArray, u32 Count);
void RenderApiDrawLines(const vertex_array *VertexArray, u32 Count);