#pragma once

#include "Types.h"

struct index_buffer;

index_buffer *CreateIndexBuffer();
index_buffer *CreateIndexBuffer(u32 *Indices, u32 Count);
void DestroyIndexBuffer(index_buffer **InexBuffer);
void BindIndexBuffer(const index_buffer *IndexBuffer);
void UnbindIndexBuffer(const index_buffer *IndexBuffer);
u32 GetIndexBufferIndexCount(const index_buffer *IndexBuffer);