#version 450 core

layout (location = 0) out vec4 FragColor;

layout (location = 0) in vec2 TexCoord;
layout (location = 1) flat in float TexSlot;

layout (binding = 0) uniform sampler2D aTextures[32];

void main()
{
    vec2 Center = vec2(0.5, 0.5);
    float Dist = distance(TexCoord, Center);

    if (Dist <= 0.5f)
    {
        FragColor = texture(aTextures[int(TexSlot)], TexCoord);
    }
    else
    {
        discard;
    }
}
