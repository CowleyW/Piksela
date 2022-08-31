#ifdef PK_OPENGL
#include "Platform/OpenGL/OpenGLVertexBuffer.hpp"
#endif

#include "Core/Core.hpp"
#include "VertexBuffer.hpp"

namespace Piksela
{

std::shared_ptr<VertexBuffer> VertexBuffer::Create(size_t size)
{
#ifdef PK_OPENGL
    return std::make_shared<OpenGLVertexBuffer>(size);
#endif
}

} // namespace Piksela