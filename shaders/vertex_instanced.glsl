#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in mat4 model;
layout (location = 6) in vec4 textureRect;

uniform mat4 projection;

out vec2 TexCoord;

void main() {
    gl_Position = projection * model * vec4(vertex_pos, 0.0f, 1.0f);

    vec2 corners[] = vec2[](
        textureRect.zw,
        textureRect.zy,
        textureRect.xw,
        textureRect.zy,
        textureRect.xy,
        textureRect.xw
    );

    frag_tex_coords = corners[gl_VertexID];
}
