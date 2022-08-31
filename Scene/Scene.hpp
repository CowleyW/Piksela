#pragma once

#include "Core/Core.hpp"
#include "Core/Input/InputEvent.hpp"
#include "Engines/Rendering/Camera.hpp"
#include "Engines/Rendering/Shader.hpp"

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

private:
    std::shared_ptr<PerspectiveCamera> mCamera;
};

} // namespace Piksela