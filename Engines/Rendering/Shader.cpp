#include "Shader.hpp"
#include "Core/Core.hpp"

namespace Piksela
{

std::shared_ptr<Shader> Shader::Create(const std::string &vertexPath, const std::string &fragmentPath)
{
#ifdef PK_OPENGL
    return nullptr;
#endif
}

} // namespace Piksela