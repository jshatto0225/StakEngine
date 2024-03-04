#shader vertex
#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in float aTexIndex;

out vec2 TexCoord;
flat out float TexSlot;

void main() {
    gl_Position = vec4(aPos, 1.0);
    TexCoord = aTexCoord;
	TexSlot = aTexIndex;
}

#shader fragment
#version 450 core
out vec4 FragColor;
  
in vec2 TexCoord;
flat in float TexSlot;

layout (binding = 0) uniform sampler2D aTextures[32];

void main() {
    FragColor = texture(aTextures[int(TexSlot)], TexCoord);
}