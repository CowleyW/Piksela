#include "Shader.hpp"
#include "Core/Core.hpp"
#include "Platform/OpenGL/OpenGLShader.hpp"

namespace Piksela
{

std::shared_ptr<Shader> Shader::Create(const std::string &vertexPath, const std::string &fragmentPath)
{
#ifdef PK_OPENGL
    return std::make_shared<OpenGLShader>(vertexPath, fragmentPath);
#endif
}

std::shared_ptr<Shader> Shader::Create(const std::string &name)
{
#ifdef PK_OPENGL
    return std::make_shared<OpenGLShader>(name);
#endif
}

} // namespace Piksela