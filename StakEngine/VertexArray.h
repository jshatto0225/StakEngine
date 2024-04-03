#pragma once

#include "Types.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

struct vertex_array;

vertex_array *CreateVertexArray();
void DestroyVertexArray(vertex_array **VertexArray);
void AddVertexBufferToVertexArray(vertex_array *VertexArray, const vertex_buffer *VertexBuffer);
void SetVertexArrayIndexBuffer(vertex_array *VertexArray, index_buffer **IndexBuffer);
void BindVertexArray(const vertex_array *VertexArray);
void UnbindVertexArray(const vertex_array *VertexArray);
const vertex_buffer_array *GetVertexArrayVertexBuffers(const vertex_array *VertexArray);
const index_buffer *GetVertexArrayIndexBuffer(const vertex_array *VertexArray);