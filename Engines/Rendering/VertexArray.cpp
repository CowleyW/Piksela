#ifdef PK_OPENGL
#include "Platform/OpenGL/OpenGLVertexArray.hpp"
#endif

#include "Core/Core.hpp"
#include "VertexArray.hpp"

namespace Piksela
{

std::shared_ptr<VertexArray> VertexArray::Create()
{
#ifdef PK_OPENGL
    return std::make_shared<OpenGLVertexArray>();
#endif
}

} // namespace Piksela