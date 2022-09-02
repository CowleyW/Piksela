#pragma once

#include "Core/Input/Input.hpp"
#include "Platform/Window.hpp"

namespace Piksela
{

class Win32Input
{
public:
    static void Init(std::shared_ptr<Window> window);
    static bool IsKeyPressed(KeyCode keycode);
    static bool IsMouseButtonPressed(MouseButton button);

private:
    static std::shared_ptr<Window> sWindow;
};

} // namespace Piksela