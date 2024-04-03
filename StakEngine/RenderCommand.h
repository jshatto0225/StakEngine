#pragma once

#include "Renderer.h"

void RenderCommandInit(window *Window);
void RenderCommandShutdown();
void RenderCommandSwapBuffers();
void RenderCommandSetClearColor(f32 Red, f32 Green, f32 Blue, f32 Alpha);
void RenderCommandClear();
void RenderCommandSetViewport(i32 X, i32 Y, i32 Width, i32 Height);
void RenderCommandSetLineWidth(f32 Width);
void RenderCommandBind();
void RenderCommandDrawIndexed(vertex_array *VertexArray, u32 Count);
void RenderCommandDrawLines(vertex_array *VertexArray, u32 Count);