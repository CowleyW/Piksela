#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "CameraController.hpp"
#include "Core/Input/Input.hpp"
//#include "Core/Input/InputEvent.hpp"

namespace Piksela
{

DebugCamera::DebugCamera() :
        mCamera(glm::vec3(0.0f, 0.0f, 2.0f), 45.0f, 0.1f, 1000.0f)
{
}

DebugCamera::~DebugCamera()
{
}

void DebugCamera::Update(float timestep)
{
    static float movementSpeed = 8.0f;

    glm::vec3 front = mCamera.GetFront();
    glm::vec3 right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
    // Forward-Backward
    if (Input::IsKeyPressed(KeyCode::W))
    {
        mCamera.Translate(movementSpeed * timestep * front);
    }
    if (Input::IsKeyPressed(KeyCode::S))
    {
        mCamera.Translate(-movementSpeed * timestep * front);
    }
    // Left-Right
    if (Input::IsKeyPressed(KeyCode::A))
    {
        mCamera.Translate(-movementSpeed * timestep * right);
    }
    if (Input::IsKeyPressed(KeyCode::D))
    {
        mCamera.Translate(movementSpeed * timestep * right);
    }
    // Up-Down
    if (Input::IsKeyPressed(KeyCode::Space))
    {
        mCamera.Translate(glm::vec3(0.0f, movementSpeed * timestep, 0.0f));
    }
    if (Input::IsKeyPressed(KeyCode::LeftShift))
    {
        mCamera.Translate(glm::vec3(0.0f, -movementSpeed * timestep, 0.0f));
    }

    // Look Left-Right
    if (Input::IsKeyPressed(KeyCode::Left))
    {
        mCamera.Rotate(glm::vec3(0.0f, -1 * timestep, 0.0f));
    }
    if (Input::IsKeyPressed(KeyCode::Right))
    {
        mCamera.Rotate(glm::vec3(0.0f, 1 * timestep, 0.0f));
    }

    // Look Up-Down
    if (Input::IsKeyPressed(KeyCode::Up))
    {
        mCamera.Rotate(glm::vec3(1 * timestep, 0.0f, 0.0f));
    }
    if (Input::IsKeyPressed(KeyCode::Down))
    {
        mCamera.Rotate(glm::vec3(-1 * timestep, 0.0f, 0.0f));
    }
}

// void DebugCamera::OnEvent(InputEvent &e)
//{
// }

} // namespace Piksela