#include "Buffer.hpp"
#include "Core/Core.hpp"

namespace Piksela
{

std::shared_ptr<VertexBuffer> VertexBuffer::Create(size_t size)
{
#ifdef PK_OPENGL
    return nullptr;
#endif
}

} // namespace Piksela