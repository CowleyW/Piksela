#include "Shader.hpp"
#include "Core/Core.hpp"

#ifdef PK_VULKAN
#include "Platform/Vulkan/VulkanShader.hpp"
#endif

namespace Piksela
{

std::shared_ptr<Shader> Shader::Create(const std::string &vertexPath, const std::string &fragmentPath)
{
#ifdef PK_VULKAN
    return std::make_shared<VulkanShader>(vertexPath, fragmentPath);
#endif
}

} // namespace Piksela