#include <glad/glad.h>

#include "OpenGLVertexBuffer.hpp"

namespace Piksela
{

OpenGLVertexBuffer::OpenGLVertexBuffer(size_t size)
{
    float vertices[] = {
        -1, -1, 1, 0.0f, 0.0f, // 0
        1, -1, 1, 1.0f, 0.0f, // 1
        -1, 1, 1, 0.0f, 1.0f, // 2
        1, 1, 1, 1.0f, 1.0f, // 3
        -1, -1, -1, 0.0f, 0.0f, // 4
        1, -1, -1, 1.0f, 0.0f, // 5
        -1, 1, -1, 0.0f, 1.0f, // 6
        1, 1, -1, 1.0f, 1.0f // 7
    };

    glCreateBuffers(1, &mVertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}

OpenGLVertexBuffer::~OpenGLVertexBuffer()
{
    glDeleteBuffers(1, &mVertexBufferID);
}

void OpenGLVertexBuffer::Bind()
{
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferID);
}

} // namespace Piksela