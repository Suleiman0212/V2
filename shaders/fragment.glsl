#version 330 core
out vec4 FragColor;
  
in vec2 TexCoord;

uniform float u_time;
uniform sampler2D ourTexture;

void main()
{
    FragColor = texture(ourTexture, TexCoord);
}
