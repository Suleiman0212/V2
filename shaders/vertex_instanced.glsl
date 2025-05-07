#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in mat4 model;
layout (location = 6) in vec4 textureRect;

uniform mat4 projection;

out vec2 texCoord;

void main() {
    gl_Position = projection * model * vec4(aPos, 0.0f, 1.0f);

    vec2 corners[] = vec2[](
        textureRect.zy,
        textureRect.zw,
        textureRect.xy,
        textureRect.zw,
        textureRect.xw,
        textureRect.xy
    );

    texCoord = corners[gl_VertexID];
}
