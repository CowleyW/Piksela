#ifdef PK_OPENGL
#include "Platform/OpenGL/OpenGLIndexBuffer.hpp"
#endif

#include "Core/Core.hpp"
#include "IndexBuffer.hpp"

namespace Piksela
{

std::shared_ptr<IndexBuffer> IndexBuffer::Create(size_t size)
{
#ifdef PK_OPENGL
    return std::make_shared<OpenGLIndexBuffer>(size);
#endif
}

} // namespace Piksela