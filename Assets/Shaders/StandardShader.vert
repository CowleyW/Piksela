#version 450

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec4 aColor;

layout(location = 2) uniform mat4 uViewProjection;

layout(location = 0) out vec4 vColor;

void main()
{
    gl_Position = uViewProjection * vec4(aPosition, 1.0f);
    vColor = aColor;
}