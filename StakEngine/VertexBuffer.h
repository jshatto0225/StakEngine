#pragma once

#include "BufferLayout.h"

struct vertex_buffer;

struct vertex_buffer_array
{
    u64 Size;
    u64 Capacity;
    const vertex_buffer **Buffers;
};

vertex_buffer *CreateVertexBuffer(u32 Size);
vertex_buffer *CreateVertexBuffer(const f32 *Vectices, u32 Size);
void DestroyVertexBuffer(vertex_buffer **VertexBuffer);
void BindVertexBuffer(const vertex_buffer *VertexBuffer);
void UnbindVertexBuffer(const vertex_buffer *VertexBuffer);
void SetVertexBufferData(vertex_buffer *VertexBuffer, const void *Data, u32 Size);
const buffer_layout *GetVertexBufferLayout(const vertex_buffer *VertexBuffer);
void SetVertexBufferLayout(vertex_buffer *VertexBuffer, buffer_layout **BufferLayout);

vertex_buffer_array *CreateVertexBufferArray(u64 Capacity);
void AddVertexBufferToArray(vertex_buffer_array *Array, const vertex_buffer *VertexBuffer);
void DestroyVertexBufferArray(vertex_buffer_array **Array);
