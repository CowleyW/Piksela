#version 450

layout(location = 0) in vec3 aPosition;
// layout(location = 1) in vec4 aColor;
layout(location = 1) in vec2 aTexCoord;

layout(location = 3) uniform mat4 uViewProjection;

layout(location = 0) out vec4 vColor;
layout(location = 1) out vec2 vTexCoord;

void main()
{
    gl_Position = uViewProjection * vec4(aPosition, 1.0f);
    vColor = vec4(1.0f, 1.0f, 1.0f, 1.0f); /*aColor*/
    vTexCoord = aTexCoord;
}