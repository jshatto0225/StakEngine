#pragma once

#include "Types.h"

enum ShaderDataType {

};

struct BufferElement {
  const char *name;
  ShaderDataType type;
  u32 size;
  u64 offset;
  bool normalized;
};

struct BufferLayout {
  BufferElement *elements;
  i32 num_elements;
  u32 stride;
};

class VertexBuffer {

};

class IndexBuffer {

};

class VertexArray {

};

class Shader {

};

class Texture {

};

class RenderApi {
public:
  static void Init();
  static void Shutdown();

  static void SetClearColor(f32 r, f32 g, f32 b, f32 a);
  // NOTE: x and y are relative to the window. (0, 0) is 
  // always the bottom left
  static void SetViewPort(i32 x, i32 y, i32 width, i32 height);
  static void SetLineWidth(f32 width);

  static void Bind();
  static void Clear();

 static void DrawIndexed(const VertexArray &vao, u32 count);
 static void DrawLines(const VertexArray &vao, u32 count);

private:
  static bool initialized;
};