#include <imgui/imgui.h>

#include "CameraController.hpp"
#include "Core/Core.hpp"
#include "Core/Input/Input.hpp"
#include "Core/Input/InputEvent.hpp"
#include "Engines/Rendering/Renderer.hpp"
#include "Engines/Rendering/Shader.hpp"
#include "Scene.hpp"

namespace Piksela
{

Scene::Scene()
{
    Renderer::SetClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    mCameraController = std::make_unique<DebugCamera>();
}

Scene::~Scene()
{
}

void Scene::Update(float timestep)
{
    mCameraController->Update(timestep);
}

void Scene::Render()
{
    Renderer::ClearBuffer();

    Renderer::BeginScene(mCameraController->GetCamera());

    Renderer::EndScene();

    glm::vec3 position = mCameraController->GetCamera().GetPosition();
    ImGui::Begin("Camera Position");
    ImGui::Text("x: %.2f", position.x);
    ImGui::Text("y: %.2f", position.y);
    ImGui::Text("z: %.2f", position.z);
    ImGui::End();
}

void Scene::OnInputEvent(InputEvent &e)
{
    mCameraController->OnEvent(e);
}

} // namespace Piksela