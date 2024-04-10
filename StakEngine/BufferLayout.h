#pragma once

#include "Types.h"
#include "ShaderDataTypes.h"

struct buffer_element
{
    const char *Name;
    shader_data_type Type;
    u32 Size;
    u64 Offset;
    bool Normalized;
};

struct buffer_layout
{
    u64 Size;
    u32 Stride;
    buffer_element *Elements;
};

u32 GetBufferElementComponentCount(const buffer_element *BufferElement);
buffer_layout *CreateBufferLayout(u64 Size);
void AddElementToLayout(buffer_layout *Layout,
                        u64 Position,
                        shader_data_type Type,
                        const char *Name,
                        bool Normalized);
void CalculateOffsetsAndStride(buffer_layout *Layout);
void DestroyBufferLayout(buffer_layout **BufferLayout);
