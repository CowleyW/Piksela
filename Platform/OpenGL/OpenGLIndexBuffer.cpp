#include <glad/glad.h>

#include "OpenGLIndexBuffer.hpp"

namespace Piksela
{

OpenGLIndexBuffer::OpenGLIndexBuffer(size_t size)
{
    mCount = size / sizeof(uint32_t);

    uint32_t indices[] = {
        // Top
        2, 6, 7,
        2, 3, 7,

        // Bottom
        0, 4, 5,
        0, 1, 5,

        // Left
        0, 2, 6,
        0, 4, 6,

        // Right
        1, 3, 7,
        1, 5, 7,

        // Front
        0, 2, 3,
        0, 1, 3,

        // Back
        4, 6, 7,
        4, 5, 7
    };

    glCreateBuffers(1, &mIndexBufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

OpenGLIndexBuffer::~OpenGLIndexBuffer()
{
    glDeleteBuffers(1, &mIndexBufferID);
}

void OpenGLIndexBuffer::Bind()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferID);
}

uint32_t OpenGLIndexBuffer::GetCount()
{
    return mCount;
}

} // namespace Piksela