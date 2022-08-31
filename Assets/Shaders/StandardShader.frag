#version 450

layout(location = 0) in vec4 vColor;
layout(location = 1) in vec2 vTexCoord;

layout(location = 0) out vec4 color;

layout(binding = 0) uniform sampler2D uTexture;

void main()
{
    color = texture(uTexture, vTexCoord);
}