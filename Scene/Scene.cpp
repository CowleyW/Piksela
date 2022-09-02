#include "Scene.hpp"
#include "CameraController.hpp"
#include "Core/Core.hpp"
#include "Core/Input/Input.hpp"
#include "Core/Input/InputEvent.hpp"
#include "Engines/Rendering/Renderer.hpp"
#include "Engines/Rendering/Shader.hpp"

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
}

void Scene::OnInputEvent(InputEvent &e)
{
}

} // namespace Piksela