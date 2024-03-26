#pragma once

#include "Types.h"
#include "StakMath.h"
#include "Font.h"
#include "Camera.h"
#include "Window.h"

enum shader_data_type {
  SDT_FLOAT,
  SDT_FLOAT2,
  SDT_FLOAT3,
  SDT_FLOAT4,
  SDT_MAT3,
  SDT_MAT4,
  SDT_INT,
  SDT_INT2,
  SDT_INT3,
  SDT_INT4,
  SDT_BOOL
};

struct buffer_element {
  const char *Name;
  shader_data_type Type;
  u32 Size;
  u64 Offset;
  bool Normalized;
};

struct buffer_layout {
  u64 Size;
  u32 Stride;
  buffer_element *Elements;
};

struct shader_source {
  char *VertexShader;
  char *FragmentShader;
};

enum shader_type {
  SHADER_TYPE_NONE,
  SHADER_TYPE_FRAGMENT,
  SHADER_TYPE_VERTEX
};

enum image_format {
  IMAGE_FORMAT_R8,
  IMAGE_FORMAT_RGB8,
  IMAGE_FORMAT_RGBA8,
  IMAGE_FORMAT_RGBA32F
};

struct texture_specification {
  i32 Width;
  i32 Height;
  image_format Format;
  bool GenerateMips;
};

struct vertex_buffer;
struct index_buffer;
struct shader;
struct vertex_array;
struct texture;
struct texture2d;
struct uniform_buffer;
struct vertex_buffer_array;

u32 ShaderDataTypeSize(shader_data_type Type);

u32 GetBufferElementComponentCount(const buffer_element *BufferElement);
buffer_layout *CreateBufferLayout(u64 Size);
void AddElementToLayout(buffer_layout *Layout,
                        u64 Position,
                        shader_data_type Type,
                        const char *Name,
                        bool Normalized);
void CalculateOffsetsAndStride(buffer_layout *Layout);
void DestroyBufferLayout(buffer_layout **BufferLayout);

vertex_buffer *CreateVertexBuffer(u32 Size);
vertex_buffer *CreateVertexBuffer(const f32 *Vectices, u32 Size);
void DestroyVertexBuffer(vertex_buffer **VertexBuffer);
void BindVertexBuffer(const vertex_buffer *VertexBuffer);
void UnbindVertexBuffer(const vertex_buffer *VertexBuffer);
void SetVertexBufferData(vertex_buffer *VertexBuffer, const void *Data, u32 Size);
const buffer_layout *GetVertexBufferLayout(const vertex_buffer *VertexBuffer);
void SetVertexBufferLayout(vertex_buffer *VertexBuffer, buffer_layout **BufferLayout);

index_buffer *CreateIndexBuffer();
index_buffer *CreateIndexBuffer(u32 *Indices, u32 Count);
void DestroyIndexBuffer(index_buffer **InexBuffer);
void BindIndexBuffer(const index_buffer *IndexBuffer);
void UnbindIndexBuffer(const index_buffer *IndexBuffer);
u32 GetIndexBufferIndexCount(const index_buffer *IndexBuffer);

vertex_array *CreateVertexArray();
void DestroyVertexArray(vertex_array **VertexArray);
void AddVertexBufferToVertexArray(vertex_array *VertexArray, const vertex_buffer *VertexBuffer);
void SetVertexArrayIndexBuffer(vertex_array *VertexArray, index_buffer **IndexBuffer);
void BindVertexArray(const vertex_array *VertexArray);
void UnbindVertexArray(const vertex_array *VertexArray);
const vertex_buffer_array *GetVertexArrayVertexBuffers(const vertex_array *VertexArray);
const index_buffer *GetVertexArrayIndexBuffer(const vertex_array *VertexArray);

shader *CreateShader(const char *VertexShader, const char *FragmentShader);
shader *CreateShader(const char *FilePath);
void DestroyShader(shader **Shader);
void CompileShader(shader *Shader, const char *VertexShader, const char *FragmentShader);
shader_source ParseShaderSource(const char *Source);
void BindShader(const shader* Shader);
void UnbindShader(const shader *Shader);

texture *CreateTexture(const texture_specification *Spec);
void DestroyTexture(texture **Texture);
void BindTexture(texture *Texture);
const texture_specification *GetTextureSpecification(const texture *Texture);
bool CompareTexture(const texture *Tex1, const texture *Tex2);

texture2d *CreateTexture2D(const char *FilePath);
texture2d *CreateTexture2D(const texture_specification *Spec);
void DestroyTexture2D(texture2d **Tex);
void SetTexture2DData(texture2d *Tex, void *Data, u32 Size);
const char *GetTexture2DFilePath(texture2d *Tex);
u32 GetRendererId(texture2d *Tex);
u32 GetTexture2DHeight(texture2d *Tex);
u32 GetTexture2DWidth(texture2d *Tex);
const texture_specification *GetTexture2DSpecification(texture2d *Tex);
void BindTexture2D(const texture2d *Tex, u32 Slot = 0);
bool IsTexture2DLoaded(const texture2d *Tex);
bool CompareTexture2D(const texture2d *Tex1, const texture2d *Tex2);

uniform_buffer *CreateUniformBuffer(u32 Size, u32 Binding);
void DestroyUniformBuffer(uniform_buffer **UniformBuffer);
void SetUniformBufferData(uniform_buffer *UniformBuffer, const void *Data, u32 Size, u32 Offset = 0);

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

void RendererInit(window *Window);
void RendererShutdown();
void RendererOnWindowResize(i32 Width, i32 Height);
void RendererBeginScene();
void RendererEndScene();
void RendererSubmit(shader *Shader, vertex_array *VertexArray);

void Renderer2DInit();
void Renderer2DBeginScene(const scene_view_camera *Cam);
void Renderer2DEndScene();
void Renderer2DShutdown();
void Renderer2DDrawQuad(const vec2 *Pos, const vec2 *Size, const vec4 *Color);
void Renderer2DDrawQuad(const vec2 *Pos,
                        const vec2 *Size,
                        texture2d *Tex,
                        bool TransferTextureOwnership);
void Renderer2DDrawQuad(const vec2 *Pos, const vec2 *Size, f32 Rotation, const vec4 *Color);
void Renderer2DDrawQuad(const vec2 *Pos,
                        const vec2 *Size,
                        f32 Rotation,
                        texture2d *Tex,
                        bool TransferTextureOwnership);
void Renderer2DDrawQuad(const mat4 *Transform, texture2d *Tex, bool TransferTextureOwnership);
