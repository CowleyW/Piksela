#include <glm/glm.hpp>

#include "CameraController.hpp"
#include "Core/Input/Input.hpp"
//#include "Core/Input/InputEvent.hpp"

namespace Piksela
{

DebugCamera::DebugCamera() :
        mCamera(glm::vec3(0.0f, 2.0f, 2.0f), 45.0f, 0.1f, 10.0f)
{
}

DebugCamera::~DebugCamera()
{
}

void DebugCamera::Update(float timestep)
{
    float delta = 8 * timestep;
    if (Input::IsKeyPressed(KeyCode::W))
    {
        mCamera.Translate(glm::vec3(0.0f, -delta, 0.0f));
    }
    if (Input::IsKeyPressed(KeyCode::A))
    {
        mCamera.Translate(glm::vec3(delta, 0.0f, 0.0f));
    }
    if (Input::IsKeyPressed(KeyCode::S))
    {
        mCamera.Translate(glm::vec3(0.0f, delta, 0.0f));
    }
    if (Input::IsKeyPressed(KeyCode::D))
    {
        mCamera.Translate(glm::vec3(-delta, 0.0f, 0.0f));
    }
}

// void DebugCamera::OnEvent(InputEvent &e)
//{
// }

} // namespace Piksela