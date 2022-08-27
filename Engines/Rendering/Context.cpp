#include "Context.hpp"
#include "Core/Core.hpp"
#include "Platform/Window.hpp"

#ifdef PK_VULKAN
#include "Platform/Vulkan/VulkanContext.hpp"
#endif

namespace Piksela
{

std::shared_ptr<Context> Context::Create(const std::shared_ptr<Window> &window)
{
#ifdef PK_VULKAN
    return std::make_shared<VulkanContext>(window);
#endif
}

} // namespace Piksela