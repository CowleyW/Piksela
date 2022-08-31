#ifdef PK_OPENGL
#include "Platform/OpenGL/OpenGLTexture.hpp"
#endif

#include "Core/Core.hpp"
#include "Texture.hpp"

namespace Piksela
{

std::shared_ptr<Texture> Texture::Create(const std::string &filePath)
{
#ifdef PK_OPENGL
    return std::make_shared<OpenGLTexture>(filePath);
#endif
}

} // namespace Piksela