#pragma once

#include "CameraController.hpp"
#include "Core/Core.hpp"
#include "Core/Input/InputEvent.hpp"
#include "Engines/Rendering/Shader.hpp"

namespace Piksela
{

class Scene
{
public:
    Scene();
    ~Scene();

    void Update(float timestep);
    void Render();

    void OnInputEvent(InputEvent &e);

private:
    std::unique_ptr<DebugCamera> mCameraController;
};

} // namespace Piksela