#pragma once

#include "Types.h"

struct uniform_buffer;

uniform_buffer *CreateUniformBuffer(u32 Size, u32 Binding);
void DestroyUniformBuffer(uniform_buffer **UniformBuffer);
void SetUniformBufferData(uniform_buffer *UniformBuffer, const void *Data, u32 Size, u32 Offset = 0);