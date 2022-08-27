#pragma once

#include "Core/Core.hpp"
#include "Core/Input/InputEvent.hpp"

namespace Piksela
{

class Scene
{
public:
    Scene();
    ~Scene();

    void Update();
    void Render();

    void OnInputEvent(InputEvent &e);
};

} // namespace Piksela