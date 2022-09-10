#include <glad/glad.h>
#include <stb/stb_image.h>

#include "Core/Core.hpp"
#include "OpenGLTexture.hpp"

namespace Piksela
{

OpenGLTexture::OpenGLTexture(const std::string &filePath)
{
    glCreateTextures(GL_TEXTURE_2D, 1, &mTextureID);

    int width, height, numChannels;
    stbi_set_flip_vertically_on_load(true);
    uint8_t *data = stbi_load(filePath.c_str(), &width, &height, &numChannels, 0);
    PK_ASSERT(data, "stbi_load failed to load texture.");

    glTextureStorage2D(mTextureID, 1, GL_RGBA8, width, height);
    glTextureSubImage2D(mTextureID, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);

    glTextureParameteri(mTextureID, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(mTextureID, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTextureParameteri(mTextureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTextureParameteri(mTextureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenerateTextureMipmap(mTextureID);

    free(data);
}

OpenGLTexture::~OpenGLTexture()
{
    glDeleteTextures(1, &mTextureID);
}

void OpenGLTexture::Bind(uint32_t slot)
{
    glBindTextureUnit(slot, mTextureID);
}

} // namespace Piksela