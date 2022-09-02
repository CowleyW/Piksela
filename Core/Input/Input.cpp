#ifdef PK_WIN32
#include "Platform/Win32/Win32Input.hpp"
#endif

#include "Input.hpp"

namespace Piksela
{

void Input::Init(std::shared_ptr<Window> window)
{
#ifdef PK_WIN32
    Win32Input::Init(window);
#endif
}

bool Input::IsKeyPressed(KeyCode keycode)
{
#ifdef PK_WIN32
    return Win32Input::IsKeyPressed(keycode);
#endif
}

bool Input::IsMouseButtonPressed(MouseButton button)
{
#ifdef PK_WIN32
    return Win32Input::IsMouseButtonPressed(button);
#endif
}

} // namespace Piksela
