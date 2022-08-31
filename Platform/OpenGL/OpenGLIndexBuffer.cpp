#include <glad/glad.h>

#include "OpenGLIndexBuffer.hpp"

namespace Piksela
{

OpenGLIndexBuffer::OpenGLIndexBuffer(size_t size)
{
    mCount = size / sizeof(uint32_t);

    uint32_t indices[] = {
        0, 1, 2, 2, 3, 0
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