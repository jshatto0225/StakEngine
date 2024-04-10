#pragma once

#include "Types.h"

enum shader_data_type
{
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

u32 ShaderDataTypeSize(shader_data_type Type);
