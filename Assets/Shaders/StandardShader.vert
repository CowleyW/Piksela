#version 450

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec4 aColor;

layout(binding = 0) uniform ModelViewProjectionMatrices
{
    mat4 Model;
    mat4 View;
    mat4 Projection;
}
uMatrices;

layout(location = 0) out vec4 vColor;

void main()
{
    gl_Position = uMatrices.Projection * uMatrices.View * uMatrices.Model * vec4(aPosition, 1.0f);
    vColor = aColor;
}