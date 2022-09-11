#pragma once

#include "Engines/Rendering/Camera.hpp"

#include "Core/Input/InputEvent.hpp"

namespace Piksela
{

class DebugCamera
{
public:
    DebugCamera();
    ~DebugCamera();

    void Update(float timestep);
    void OnEvent(InputEvent &e);

    PerspectiveCamera &GetCamera()
    {
        return mCamera;
    }

private:
    PerspectiveCamera mCamera;
};

} // namespace Piksela