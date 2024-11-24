#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in float aTexIndex;

layout (std140, binding = 0) uniform Camera
{
    mat4 u_ViewProj;
};

layout (location = 0) out vec2 TexCoord;
layout (location = 1) flat out float TexSlot;

void main()
{
    gl_Position = u_ViewProj * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
    TexSlot = aTexIndex;
}
