#include "Core/Core.hpp"
#include "Engines/Rendering/Texture.hpp"

namespace Piksela
{

class OpenGLTexture : public Texture
{
public:
    OpenGLTexture(const std::string &filePath);
    ~OpenGLTexture();

    virtual void Bind(uint32_t slot) override;

private:
    uint32_t mTextureID;
};

} // namespace Piksela