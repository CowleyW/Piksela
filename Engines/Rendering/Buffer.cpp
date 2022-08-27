#ifdef PK_VULKAN
#include "Platform/Vulkan/VulkanBuffer.hpp"
#endif

#include "Buffer.hpp"
#include "Core/Core.hpp"

namespace Piksela
{

std::shared_ptr<VertexBuffer> VertexBuffer::Create(size_t size)
{
#ifdef PK_VULKAN
    return std::make_shared<VulkanVertexBuffer>(size);
#endif
}

} // namespace Piksela