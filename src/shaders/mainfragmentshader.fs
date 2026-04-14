#version 330 core


out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D currentTexture;

void main()
{
    vec4 texColor = texture(currentTexture, TexCoord);
    FragColor = texColor;
}