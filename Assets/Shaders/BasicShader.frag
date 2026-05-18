#version 450

#extension GL_EXT_buffer_reference : require
#extension GL_EXT_scalar_block_layout : require

layout(location = 0) in vec3 fragColor;

layout(location = 0) out vec4 outColor;

layout(buffer_reference, scalar) readonly buffer VertData {
    uint data[];
};

layout(buffer_reference, scalar) readonly buffer FragData {
    uint data[];
};

layout(push_constant, scalar) uniform RasterData {
    VertData vert_data;
    FragData frag_data;
} push_constants;

void main() {
    outColor = vec4(fragColor, 1.0);
}
