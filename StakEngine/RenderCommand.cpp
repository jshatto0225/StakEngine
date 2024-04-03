#include "RenderCommand.h"

#include "RenderApi.h"

/********************
 * Public Interface *
 ********************/

void
RenderCommandInit(window *Window)
{
  RenderApiInit(Window);
}

void
RenderCommandShutdown()
{
  RenderApiShutdown();
}

void
RenderCommandSwapBuffers()
{
  RenderApiSwapBuffers();
}

void
RenderCommandSetClearColor(f32 Red, f32 Green, f32 Blue, f32 Alpha)
{
  RenderApiSetClearColor(Red, Green, Blue, Alpha);
}

void
RenderCommandClear() {
  RenderApiClear();
}

void
RenderCommandSetViewport(i32 X, i32 Y, i32 Width, i32 Height)
{
  RenderApiSetViewport(X, Y, Width, Height);
}

void
RenderCommandSetLineWidth(f32 Width)
{
  RenderApiSetLineWidth(Width);
}

void
RenderCommandBind() {
  RenderApiBind();
}

void
RenderCommandDrawIndexed(vertex_array *VertexArray, u32 Count)
{
  RenderApiDrawIndexed(VertexArray, Count);
}

void
RenderCommandDrawLines(vertex_array *VertexArray, u32 Count)
{
  RenderApiDrawLines(VertexArray, Count);
}