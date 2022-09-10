#include <glad/glad.h>

#include "OpenGLVertexArray.hpp"

namespace Piksela
{

OpenGLVertexArray::OpenGLVertexArray()
{
    glCreateVertexArrays(1, &mVertexArrayID);
}

OpenGLVertexArray::~OpenGLVertexArray()
{
    glDeleteVertexArrays(1, &mVertexArrayID);
}

void OpenGLVertexArray::Bind()
{
    glBindVertexArray(mVertexArrayID);
}

void OpenGLVertexArray::Unbind()
{
    glBindVertexArray(0);
}

void OpenGLVertexArray::SetVertexBuffer(const std::shared_ptr<VertexBuffer> &vertexBuffer)
{
    glBindVertexArray(mVertexArrayID);
    vertexBuffer->Bind();

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    // glEnableVertexAttribArray(1);
    // glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));

    mVertexBuffer = vertexBuffer;
}

void OpenGLVertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer> &indexBuffer)
{
    glBindVertexArray(mVertexArrayID);
    indexBuffer->Bind();

    mIndexBuffer = indexBuffer;
}

std::shared_ptr<VertexBuffer> OpenGLVertexArray::GetVertexBuffer()
{
    return mVertexBuffer;
}

std::shared_ptr<IndexBuffer> OpenGLVertexArray::GetIndexBuffer()
{
    return mIndexBuffer;
}

} // namespace Piksela