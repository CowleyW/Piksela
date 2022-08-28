#include "Context.hpp"
#include "Core/Core.hpp"
#include "Platform/Window.hpp"

#ifdef PK_OPENGL
#include "Platform/OpenGL/OpenGLContext.hpp"
#endif

namespace Piksela
{

std::shared_ptr<Context> Context::Create(const std::shared_ptr<Window> &window)
{
#ifdef PK_OPENGL
    return std::make_shared<OpenGLContext>(window);
#endif
}

} // namespace Piksela