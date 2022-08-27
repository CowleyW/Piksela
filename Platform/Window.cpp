#include "Window.hpp"

#ifdef PK_WIN32
#include "Win32/Win32Window.hpp"
#endif

namespace Piksela
{

std::shared_ptr<Window> Window::InitWindow(WindowSpecification specification)
{
#ifdef PK_WIN32
    return std::make_shared<Win32Window>(specification);
#endif
}

Window::~Window()
{
}

Window::Window(WindowSpecification specification) :
        mSpecification(specification)
{
}

void Window::SetWidth(uint32_t width)
{
    mSpecification.WindowWidth = width;
}

void Window::SetHeight(uint32_t height)
{
    mSpecification.WindowHeight = height;
}

} // namespace Piksela