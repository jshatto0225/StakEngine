#pragma once

#include "Types.h"

enum shader_type
{
  SHADER_TYPE_NONE,
  SHADER_TYPE_FRAGMENT,
  SHADER_TYPE_VERTEX
};

struct shader_source
{
  char *VertexShader;
  char *FragmentShader;
};

struct shader;

shader *CreateShader(const char *VertexShader, const char *FragmentShader);
shader *CreateShader(const char *FilePath);
void DestroyShader(shader **Shader);
void CompileShader(shader *Shader, const char *VertexShader, const char *FragmentShader);
shader_source ParseShaderSource(const char *Source);
void BindShader(const shader *Shader);
void UnbindShader(const shader *Shader);