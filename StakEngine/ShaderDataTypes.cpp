#include "ShaderDataTypes.h"

u32
ShaderDataTypeSize(shader_data_type Type)
{
  switch (Type)
  {
  case SDT_FLOAT:
    return 4;
  case SDT_FLOAT2:
    return 4 * 2;
  case SDT_FLOAT3:
    return 4 * 3;
  case SDT_FLOAT4:
    return 4 * 4;
  case SDT_MAT3:
    return 4 * 3 * 3;
  case SDT_MAT4:
    return 4 * 4 * 4;
  case SDT_INT:
    return 4;
  case SDT_INT2:
    return 4 * 2;
  case SDT_INT3:
    return 4 * 3;
  case SDT_INT4:
    return 4 * 4;
  case SDT_BOOL:
    return 1;
  }
  return 0;
}