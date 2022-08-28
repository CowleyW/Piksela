#include "Scene.hpp"
#include "Core/Core.hpp"
#include "Core/Input/InputEvent.hpp"
#include "Engines/Rendering/Renderer.hpp"

namespace Piksela
{

Scene::Scene()
{
    Renderer::SetClearColor(0.1f, 0.1f, 0.1f, 1.0f);
}

Scene::~Scene()
{
}

void Scene::Update()
{
}

void Scene::Render()
{
    Renderer::ClearBuffer();

    Renderer::BeginScene();
    Renderer::EndScene();
}

void Scene::OnInputEvent(InputEvent &e)
{
}

} // namespace Piksela