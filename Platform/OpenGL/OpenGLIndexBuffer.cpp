#include <glad/glad.h>

#include "OpenGLIndexBuffer.hpp"

namespace Piksela
{

OpenGLIndexBuffer::OpenGLIndexBuffer(size_t size)
{
    uint32_t indices[] = {
        0, 1, 2, 2, 3, 0
    };

    glGenBuffers(1, &mIndexBufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

void OpenGLIndexBuffer::Bind()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferID);
}

} // namespace Piksela