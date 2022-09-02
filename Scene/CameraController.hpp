#pragma once

//#include "Core/Input/InputEvent.hpp"
#include "Engines/Rendering/Camera.hpp"

namespace Piksela
{

class DebugCamera
{
public:
    DebugCamera();
    ~DebugCamera();

    void Update(float timestep);
    // void OnEvent(InputEvent &e);

    const PerspectiveCamera &GetCamera()
    {
        return mCamera;
    }

private:
    PerspectiveCamera mCamera;
};

} // namespace Piksela