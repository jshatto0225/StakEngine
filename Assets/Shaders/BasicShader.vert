#version 450

#extension GL_EXT_buffer_reference : require
#extension GL_EXT_scalar_block_layout : require

layout(location = 0) out vec3 fragColor;

struct Vertex {
    vec3 pos;
    vec4 color;
};

layout(buffer_reference, scalar) readonly buffer VertexBuffer {
    Vertex vertices[];
};

layout(buffer_reference, scalar) readonly buffer FragData {
    uint data[];
};

layout(push_constant, scalar) uniform RasterData {
    VertexBuffer vertex_buffer;
    FragData frag_data;
} push_constants;

void main() {
    gl_Position = vec4(push_constants.vertex_buffer.vertices[gl_VertexIndex].pos, 1.0);
    fragColor = push_constants.vertex_buffer.vertices[gl_VertexIndex].color;
}
