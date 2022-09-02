#pragma once

#include "KeyCode.hpp"
#include "Platform/Window.hpp"

namespace Piksela
{

class Input
{
public:
    static void Init(std::shared_ptr<Window> window);

    static bool IsKeyPressed(KeyCode keycode);
    static bool IsMouseButtonPressed(MouseButton button);
};

} // namespace Piksela